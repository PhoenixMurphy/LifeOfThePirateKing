#include "raylib.h"

#include "core/Config.h"
#include "world/Arena.h"
#include "world/Layouts.h"
#include "entities/Player.h"
#include "entities//BulletManager.h"

static void DrawHUD(const Player& player, float timeLeft, std::size_t bulletCount)
{
    // Bar at the top of the screen
    DrawRectangle(0, 0, Config::ScreenWidth, Config::HUDHeight, Color{ 30, 22, 18, 255 });

    DrawText(TextFormat("Lives: %d", player.Lives()), 16, 12, 20, RAYWHITE);
    DrawText(TextFormat("Coins: %d", player.Coins()), 140, 12, 20, GOLD);
    DrawText(TextFormat("Bullets: %d", static_cast<int>(bulletCount)), 270, 12, 20, LIGHTGRAY);

    const char* timeText = TextFormat("Time: %d", static_cast<int>(timeLeft + 0.99f));  // round up
    DrawText(timeText, Config::ScreenWidth - MeasureText(timeText, 20) - 16, 12, 20, RAYWHITE);

    // Timer bar at the bottom
    const float fraction = timeLeft / Config::LevelDuration;
    DrawRectangle(16, 44, Config::ScreenWidth - 32, 8, Color{ 70, 55, 45, 255 });
    DrawRectangle(16, 44, static_cast<int>((Config::ScreenWidth - 32) * fraction), 8, ORANGE);
}

static void DrawDebugOverlay(const Arena& arena)
{
    const Vector2 mouse = GetMousePosition();
    const int tx = arena.WorldToTileX(mouse.x);
    const int ty = arena.WorldToTileY(mouse.y);

    if (arena.InBounds(tx, ty))
    {
        DrawRectangleLinesEx(arena.TileRect(tx, ty), 3.0f, RED);

        const char* text = TextFormat("tile (%d, %d)   index = %d * %d + %d = %d   solid: %s",
            tx, ty, ty, arena.Width(), tx, arena.Index(tx, ty),
            arena.IsSolid(tx, ty) ? "yes" : "no");
        DrawRectangle(0, Config::ScreenHeight - 28, Config::ScreenWidth, 28, Fade(BLACK, 0.7f));
        DrawText(text, 10, Config::ScreenHeight - 23, 18, RAYWHITE);
    }

    DrawFPS(Config::ScreenWidth - 90, Config::HUDHeight + 8);
}

int main()
{
    InitWindow(Config::ScreenWidth, Config::ScreenHeight, Config::WindowTitle);
    SetTargetFPS(Config::FPS);

    ChangeDirectory(GetApplicationDirectory());

    Arena arena;
    arena.LoadFromStrings(Layouts::Level1);

    Player player;
    player.Reset(arena.PlayerStart());

    BulletManager bullets;

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

        player.Update(dt, arena, bullets);
        bullets.Update(dt, arena);

        timeLeft -= dt;
        if (timeLeft < 0.0f)
        {
            timeLeft = 0.0f;
        }
           
        BeginDrawing();
        ClearBackground(BLACK);

        arena.Draw();
        bullets.Draw();
        player.Draw();
        DrawHUD(player, timeLeft, bullets.Count());

        if (showDebug)
        {
            DrawDebugOverlay(arena);
        }

        EndDrawing();
    }

    CloseWindow();
}