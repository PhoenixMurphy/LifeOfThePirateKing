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
#include "world/Layouts.h"

static void DrawHUD(const Player& player, float timeLeft, int wave)
{
    // Bar at the top of the screen
    DrawRectangle(0, 0, Config::ScreenWidth, Config::HUDHeight, Color{ 30, 22, 18, 255 });

    DrawText(TextFormat("Lives: %d", player.Lives()), 16, 12, 20, RAYWHITE);
    DrawText(TextFormat("Coins: %d", player.Coins()), 140, 12, 20, GOLD);
    DrawText(TextFormat("Wave: %d", wave), 270, 12, 20, LIGHTGRAY);

    const char* timeText = TextFormat("Time: %d", static_cast<int>(timeLeft + 0.99f));  // round up
    DrawText(timeText, Config::ScreenWidth - MeasureText(timeText, 20) - 16, 12, 20, RAYWHITE);

    // Timer bar at the bottom
    const float fraction = timeLeft / Config::LevelDuration;
    DrawRectangle(16, 44, Config::ScreenWidth - 32, 8, Color{ 70, 55, 45, 255 });
    DrawRectangle(16, 44, static_cast<int>((Config::ScreenWidth - 32) * fraction), 8, ORANGE);
}

// Big message in the middle of the arena
static void DrawBanner(const char* title, const char* subtitle)
{
    const int centreY = Config::HUDHeight + (Config::ScreenHeight - Config::HUDHeight) / 2;
    DrawRectangle(0, centreY - 50, Config::ScreenWidth, 100, Fade(BLACK, 0.75f));
    DrawText(title, (Config::ScreenWidth - MeasureText(title, 40)) / 2, centreY - 36, 40, GOLD);
    DrawText(subtitle, (Config::ScreenWidth - MeasureText(subtitle, 20)) / 2, centreY + 14, 20, RAYWHITE);
}

static void DrawDebugOverlay(const Arena& arena, std::size_t bulletCount, std::size_t enemyCount)
{
    const Vector2 mouse = GetMousePosition();
    const int tx = arena.WorldToTileX(mouse.x);
    const int ty = arena.WorldToTileY(mouse.y);

    DrawRectangle(0, Config::ScreenHeight - 48, Config::ScreenWidth, 48, Fade(BLACK, 0.7f));
    DrawText(TextFormat("bullets: %d   enemies: %d",
        static_cast<int>(bulletCount), static_cast<int>(enemyCount)),
        10, Config::ScreenHeight - 44, 16, RAYWHITE);

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

    ChangeDirectory(GetApplicationDirectory());

    const Definitions defs;

    Arena arena;
    arena.LoadFromStrings(Layouts::Level1);

    Player player;
    player.Reset(arena.PlayerStart());

    BulletManager bullets;
    EnemyManager  enemies;

    WaveSpawner spawner;
    spawner.SetGates(arena.Gates());
    spawner.StartWave(1, 1.0f);

    float timeLeft = Config::LevelDuration;
    bool  showDebug = false;

    while (!WindowShouldClose())
    {
        float dt = GetFrameTime();
        if (dt > Config::MaxFPSTime)
        {
            dt = Config::MaxFPSTime;
        }

        if (IsKeyPressed(KEY_F1))
        {
            showDebug = !showDebug;
        }

        // --- Game over: Enter starts a new game ---
        if (!player.IsAlive())
        {
            if (IsKeyPressed(KEY_ENTER))
            {
                player.Reset(arena.PlayerStart());
                bullets.Clear();
                enemies.Clear();
                spawner.StartWave(1, 1.0f);
                timeLeft = Config::LevelDuration;
            }
        }
        else
        {
            // --- Player and shooting: exactly as in step 3 ---
            player.Update(dt, arena, bullets);
            bullets.Update(dt, arena);

            // --- Enemies ---
            if (timeLeft > 0.0f)
            {
                spawner.Update(dt, enemies, defs);
            }
            enemies.Update(dt, player.Position(), arena);

            // Bullets hit enemies; kills pay coins
            player.AddCoins(Collision::BulletsVsEnemies(bullets, enemies));
            enemies.RemoveDead();

            // Touching an enemy costs a life and clears the arena
            if (Collision::EnemyTouchesPlayer(enemies, player))
            {
                player.LoseLife();
                enemies.Clear();
                bullets.Clear();
                player.Respawn(arena.PlayerStart());
                spawner.RestartWave(Config::RespawnDelay);
            }

            timeLeft -= dt;
            if (timeLeft < 0.0f)
            {
                timeLeft = 0.0f;
            }
        }

        BeginDrawing();
        ClearBackground(BLACK);

        arena.Draw();
        enemies.Draw();
        bullets.Draw();
        player.Draw();
        DrawHUD(player, timeLeft, spawner.WaveNumber());

        if (!player.IsAlive())
        {
            DrawBanner("GAME OVER", TextFormat("Coins: %d   Press Enter to play again", player.Coins()));
        }
        else if (timeLeft <= 0.0f && enemies.Count() == 0)
        {
            DrawBanner("LEVEL CLEAR", " ");
        }

        if (showDebug)
        {
            DrawDebugOverlay(arena, bullets.Count(), enemies.Count());
        }

        EndDrawing();
    }

    CloseWindow();
    return 0;
}