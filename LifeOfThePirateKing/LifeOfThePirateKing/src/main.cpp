#include <string>

#include "raylib.h"

#include "core/Config.h"
#include "data/Definitions.h"
#include "entities/BulletManager.h"
#include "entities/EnemyManager.h"
#include "entities/Player.h"
#include "systems/Collision.h"
#include "systems/WaveSpawner.h"
#include "world/Arena.h"
#include "world/LevelSequence.h"
#include "ui/HighScoreTable.h"
#include "ui/Shop.h"

// Where the high scores are saved, next to the .exe.
static const char* const HighScoreFile = "highscores.txt";

// What the game is doing right now. Each state has its own update rules.
enum class GameState
{
    Playing,
    LevelClear,
    Shop,
    GameOver,
    Victory,
    EnterName,
    HighScores
};

// The strip across the top: lives, coins, level name, then the timer or the
// boss's health underneath.
static void DrawHud(const Player& player, const LevelInfo* level, float timeLeft, const Enemy* boss)
{
    DrawRectangle(0, 0, Config::ScreenWidth, Config::HUDHeight, Color{ 30, 22, 18, 255 });

    DrawText(TextFormat("Lives: %d", player.Lives()), 16, 12, 20, RAYWHITE);
    DrawText(TextFormat("Coins: %d", player.Coins()), 120, 12, 20, GOLD);
    if (level != nullptr)
        DrawText(level->name.c_str(), 240, 12, 20, LIGHTGRAY);

    // Background of the bar
    DrawRectangle(16, 44, Config::ScreenWidth - 32, 8, Color{ 70, 55, 45, 255 });

    if (level != nullptr && level->isBoss)
    {
        // Boss levels: no timer, the bar shows the boss's health instead.
        const char* bossText = "BOSS";
        DrawText(bossText, Config::ScreenWidth - MeasureText(bossText, 20) - 16, 12, 20, RED);
        if (boss != nullptr)
        {
            const float fraction = static_cast<float>(boss->health) / static_cast<float>(boss->maxHealth);
            DrawRectangle(16, 44, static_cast<int>((Config::ScreenWidth - 32) * fraction), 8, RED);
        }
    }
    else if (level != nullptr)
    {
        const char* timeText = TextFormat("Time: %d", static_cast<int>(timeLeft + 0.99f));  // round up
        DrawText(timeText, Config::ScreenWidth - MeasureText(timeText, 20) - 16, 12, 20, RAYWHITE);

        const float fraction = timeLeft / level->timeLimit;
        DrawRectangle(16, 44, static_cast<int>((Config::ScreenWidth - 32) * fraction), 8, ORANGE);
    }
}

// Big message in the middle of the arena: a title and two lines.
static void DrawBanner(const char* title, const char* line1, const char* line2)
{
    const int centreY = Config::HUDHeight + (Config::ScreenHeight - Config::HUDHeight) / 2;
    DrawRectangle(0, centreY - 64, Config::ScreenWidth, 128, Fade(BLACK, 0.8f));
    DrawText(title, (Config::ScreenWidth - MeasureText(title, 40)) / 2, centreY - 50, 40, GOLD);
    DrawText(line1, (Config::ScreenWidth - MeasureText(line1, 20)) / 2, centreY + 4, 20, RAYWHITE);
    DrawText(line2, (Config::ScreenWidth - MeasureText(line2, 20)) / 2, centreY + 32, 20, LIGHTGRAY);
}

// F1: where you are in the run, live counts, and the tile under the mouse.
static void DrawDebugOverlay(const Arena& arena, const LevelSequence& levels, int wave,
    std::size_t bulletCount, std::size_t enemyCount)
{
    DrawRectangle(0, Config::ScreenHeight - 48, Config::ScreenWidth, 48, Fade(BLACK, 0.7f));
    DrawText(TextFormat("level %d of %d   wave %d   bullets %d   enemies %d",
        levels.Number(), static_cast<int>(levels.Count()), wave,
        static_cast<int>(bulletCount), static_cast<int>(enemyCount)),
        10, Config::ScreenHeight - 44, 16, RAYWHITE);

    const Vector2 mouse = GetMousePosition();
    const int tx = arena.WorldToTileX(mouse.x);
    const int ty = arena.WorldToTileY(mouse.y);
    if (arena.InBounds(tx, ty))
    {
        const char* solidText = "no";
        if (arena.IsSolid(tx, ty))
            solidText = "yes";

        DrawRectangleLinesEx(arena.TileRect(tx, ty), 3.0f, RED);
        DrawText(TextFormat("tile (%d, %d)  index = %d * %d + %d = %d  solid: %s",
            tx, ty, ty, arena.Width(), tx, arena.Index(tx, ty), solidText),
            10, Config::ScreenHeight - 22, 16, RAYWHITE);
    }

    DrawFPS(Config::ScreenWidth - 90, Config::HUDHeight + 8);
}

int main()
{
    InitWindow(Config::ScreenWidth, Config::ScreenHeight, Config::WindowTitle);
    SetTargetFPS(Config::FPS);

    // Load files relative to the .exe, so "assets/..." paths always work.
    ChangeDirectory(GetApplicationDirectory());

    const Definitions defs;

    HighScoreTable highScores;
    if (!highScores.Load(HighScoreFile))
        TraceLog(LOG_INFO, "No high score file yet - starting a new table");

    Shop        shop;
    std::string nameEntry;          // what the player has typed so far

    LevelSequence levels;
    levels.Build();

    Arena         arena;
    Player        player;
    BulletManager bullets;
    EnemyManager  enemies;
    WaveSpawner   spawner;

    GameState state = GameState::Playing;
    float     timeLeft = 0.0f;
    bool      showDebug = false;

    // Brings in the boss at its start tile with this level's health.
    // Spawn returns a reference to the new enemy, so we can change it.
    auto spawnBoss = [&]()
        {
            Enemy& boss = enemies.Spawn(EnemyType::Boss, arena.BossStart(), defs);
            boss.health = levels.Current()->bossHealth;
            boss.maxHealth = boss.health;
        };

    // Sets up whichever level is current. Lives and coins carry over.
    auto startLevel = [&]()
        {
            const LevelInfo& level = *levels.Current();

            arena.LoadFromStrings(level.layout);
            arena.SetStage(level.stage);
            spawner.SetGates(arena.Gates());
            spawner.StartWave(level.firstWave, 1.0f);

            bullets.Clear();
            enemies.Clear();
            player.Respawn(arena.PlayerStart());
            timeLeft = level.timeLimit;

            if (level.isBoss)
                spawnBoss();

            state = GameState::Playing;
        };

    // Back to level 1 with full lives, no coins and a full shop.
    // The shop is only restocked here, so stock carries over between bosses.
    auto startNewGame = [&]()
        {
            levels.Restart();
            shop.Restock();
            player.Reset(arena.PlayerStart());   // startLevel then moves you to the right spot
            startLevel();
        };
    startNewGame();

    while (!WindowShouldClose())
    {
        float dt = GetFrameTime();
        if (dt > Config::MaxFPSTime)
            dt = Config::MaxFPSTime;

        if (IsKeyPressed(KEY_F1))
            showDebug = !showDebug;

        // nullptr only in the Victory state, after the last level.
        const LevelInfo* level = levels.Current();

        // --- Update: what happens depends on the state ----------------------
        switch (state)
        {
        case GameState::Playing:
        {
            // Debug: F2 skips straight to "level clear" (handy for testing
            // later levels, or reaching a boss quickly in a demo).
            if (IsKeyPressed(KEY_F2))
            {
                enemies.Clear();
                bullets.Clear();
                state = GameState::LevelClear;
                break;
            }

            player.Update(dt, arena, bullets);
            bullets.Update(dt, arena);

            // Enemies keep coming until time runs out - or, on a boss
            // level, until the boss is dead.
            const Enemy* boss = enemies.FindFirst(EnemyType::Boss);
            bool spawning = false;
            if (level->isBoss)
                spawning = (boss != nullptr);
            else
                spawning = (timeLeft > 0.0f);

            if (spawning)
                spawner.Update(dt, enemies, defs);

            enemies.Update(dt, player.Position(), arena);

            // Kills pay coins, multiplied on the bonus level.
            player.AddCoins(Collision::BulletsVsEnemies(bullets, enemies) * level->coinBonus);
            enemies.RemoveDead();

            // Touching an enemy costs a life and resets the arena.
            if (Collision::EnemyTouchesPlayer(enemies, player))
            {
                player.LoseLife();
                enemies.Clear();
                bullets.Clear();
                player.Respawn(arena.PlayerStart());
                spawner.RestartWave(Config::RespawnDelay);
                if (level->isBoss)
                    spawnBoss();                 // back at full health
            }

            if (!level->isBoss)
            {
                timeLeft -= dt;
                if (timeLeft < 0.0f)
                    timeLeft = 0.0f;
            }

            // Has the level ended?
            if (!player.IsAlive())
            {
                state = GameState::GameOver;
            }
            else
            {
                bool goalMet = false;
                if (level->isBoss)
                    goalMet = (enemies.FindFirst(EnemyType::Boss) == nullptr);   // boss is dead
                else
                    goalMet = (timeLeft <= 0.0f);                               // survived the timer

                if (goalMet && enemies.Count() == 0)                            // and the arena is clear
                    state = GameState::LevelClear;
            }
            break;
        }

        case GameState::LevelClear:
            if (IsKeyPressed(KEY_ENTER))
            {
                if (levels.Next() == nullptr)
                {
                    state = GameState::Victory;  // that was the last level
                }
                else if (level->isBoss)
                {
                    state = GameState::Shop;     // a shop after every boss
                }
                else
                {
                    levels.Advance();            // normal level: straight on to the next one
                    startLevel();
                }
            }
            break;

        case GameState::Shop:
            if (shop.Update(player))             // true when the player leaves
            {
                levels.Advance();                // follow the list to the next level
                startLevel();
            }
            break;

        case GameState::GameOver:
        case GameState::Victory:
            if (IsKeyPressed(KEY_ENTER))
            {
                if (highScores.Qualifies(player.Score()))
                {
                    nameEntry.clear();
                    state = GameState::EnterName;
                }
                else
                {
                    state = GameState::HighScores;
                }
            }
            break;

        case GameState::EnterName:
        {
            // GetCharPressed gives each typed character once (it handles
            // Shift, so 'a' and 'A' both work). 0 means nothing left.
            int key = GetCharPressed();
            while (key > 0)
            {
                if (key >= 32 && key <= 126 && nameEntry.size() < 12)
                    nameEntry += static_cast<char>(key);
                key = GetCharPressed();
            }
            if (IsKeyPressed(KEY_BACKSPACE) && !nameEntry.empty())
                nameEntry.pop_back();

            if (IsKeyPressed(KEY_ENTER))
            {
                std::string name = nameEntry;
                if (name.empty())
                    name = "Pirate";               // nothing typed: use a default

                highScores.Add(player.Score(), name);
                if (!highScores.Save(HighScoreFile))
                    TraceLog(LOG_WARNING, "Could not save high scores to %s", HighScoreFile);
                state = GameState::HighScores;
            }
            break;
        }

        case GameState::HighScores:
            if (IsKeyPressed(KEY_ENTER))
                startNewGame();
            break;
        }

        // --- Draw (back to front) -------------------------------------------
        BeginDrawing();
        ClearBackground(BLACK);

        arena.Draw();
        enemies.Draw();
        bullets.Draw();
        player.Draw();
        DrawHud(player, level, timeLeft, enemies.FindFirst(EnemyType::Boss));

        if (state == GameState::LevelClear)
        {
            const LevelInfo* next = levels.Next();

            const char* title = "LEVEL CLEAR";
            const char* prompt = "Press Enter to finish";
            if (level->isBoss)
                title = "BOSS DEFEATED";

            if (next != nullptr && level->isBoss)
                prompt = "Press Enter for the shop";
            else if (next != nullptr)
                prompt = TextFormat("Press Enter for %s", next->name.c_str());

            DrawBanner(title, TextFormat("Coins: %d", player.Coins()), prompt);
        }
        else if (state == GameState::Shop)
        {
            shop.Draw(player);
        }
        else if (state == GameState::GameOver)
        {
            DrawBanner("GAME OVER", TextFormat("Score: %d", player.Score()), "Press Enter");
        }
        else if (state == GameState::Victory)
        {
            DrawBanner("VICTORY", TextFormat("Pirate King! Score: %d", player.Score()), "Press Enter");
        }
        else if (state == GameState::EnterName)
        {
            // Blinking cursor: on for half a second, off for half a second
            const char* cursor = " ";
            if (static_cast<int>(GetTime() * 2.0) % 2 == 0)
                cursor = "_";

            DrawBanner("NEW HIGH SCORE!",
                TextFormat("Name: %s%s", nameEntry.c_str(), cursor),
                "Type your name, then Enter");
        }
        else if (state == GameState::HighScores)
        {
            highScores.Draw();
        }

        if (showDebug)
            DrawDebugOverlay(arena, levels, spawner.WaveNumber(), bullets.Count(), enemies.Count());

        EndDrawing();
    }

    CloseWindow();
    return 0;
}