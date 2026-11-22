// Life of The Pirate King - entry point and main loop (step 7)
#include <cstddef>

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

// What the game is doing right now. Each state has its own update rules.
enum class GameState
{
    Playing,      // in a level
    LevelClear,   // level beaten, waiting for Enter (the shop goes here in step 8)
    GameOver,     // out of lives, waiting for Enter
    Victory       // every level beaten, waiting for Enter
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
        DrawRectangleLinesEx(arena.TileRect(tx, ty), 3.0f, RED);
        DrawText(TextFormat("tile (%d, %d)  index = %d * %d + %d = %d  solid: %s",
            tx, ty, ty, arena.Width(), tx, arena.Index(tx, ty),
            arena.IsSolid(tx, ty) ? "yes" : "no"),
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

    // Back to level 1 with full lives and no coins.
    auto startNewGame = [&]()
        {
            levels.Restart();
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
            const bool   spawning = level->isBoss ? (boss != nullptr) : (timeLeft > 0.0f);
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
                const bool goalMet = level->isBoss
                    ? (enemies.FindFirst(EnemyType::Boss) == nullptr)   // boss is dead
                    : (timeLeft <= 0.0f);                               // survived the timer
                if (goalMet && enemies.Count() == 0)                    // and the arena is clear
                    state = GameState::LevelClear;
            }
            break;
        }

        case GameState::LevelClear:
            if (IsKeyPressed(KEY_ENTER))
            {
                // (Step 8 opens the shop here, before the next level.)
                if (levels.Advance())
                    startLevel();                // follow the list to the next level
                else
                    state = GameState::Victory;  // that was the last one
            }
            break;

        case GameState::GameOver:
        case GameState::Victory:
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
            DrawBanner(level->isBoss ? "BOSS DEFEATED" : "LEVEL CLEAR",
                TextFormat("Coins: %d", player.Coins()),
                next ? TextFormat("Press Enter for %s", next->name.c_str())
                : "Press Enter to finish");
        }
        else if (state == GameState::GameOver)
        {
            DrawBanner("GAME OVER", TextFormat("Coins: %d", player.Coins()), "Press Enter to play again");
        }
        else if (state == GameState::Victory)
        {
            DrawBanner("VICTORY", TextFormat("Pirate King! Coins: %d", player.Coins()), "Press Enter to play again");
        }

        if (showDebug)
            DrawDebugOverlay(arena, levels, spawner.WaveNumber(), bullets.Count(), enemies.Count());

        EndDrawing();
    }

    CloseWindow();
    return 0;
}