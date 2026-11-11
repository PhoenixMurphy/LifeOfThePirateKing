#include "raylib.h"

#include "core/Config.h"
#include "world/Arena.h"
#include "world/Layouts.h"

int main()
{
    InitWindow(Config::ScreenWidth, Config::ScreenHeight, Config::WindowTitle);
    SetTargetFPS(Config::MaxFPSTime);
    ChangeDirectory(GetApplicationDirectory());

    Arena arena;
    arena.LoadFromStrings(Layouts::Level1);

    while (!WindowShouldClose())
    {
        BeginDrawing();
        ClearBackground(BLACK);

        arena.Draw();

        // This is where the player will spawn
        DrawCircleV(arena.PlayerStart(), 10.0f, RED);

        // Show the tile under the mouse and its vector index
        const Vector2 mouse = GetMousePosition();
        const int tx = arena.WorldToTileX(mouse.x);
        const int ty = arena.WorldToTileY(mouse.y);
        if (arena.InBounds(tx, ty))
        {
            DrawRectangleLinesEx(arena.TileRect(tx, ty), 3.0f, RED);
            DrawText(TextFormat("tile (%d, %d)  index = %d  solid: %s",
                tx, ty, arena.Index(tx, ty),
                arena.IsSolid(tx, ty) ? "yes" : "no"),
                10, 20, 20, RAYWHITE);
        }

        EndDrawing();
    }

    CloseWindow();
    return 0;
}