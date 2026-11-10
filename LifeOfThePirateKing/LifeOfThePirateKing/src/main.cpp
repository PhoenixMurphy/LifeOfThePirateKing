#include "raylib.h"

int main()
{
    InitWindow(1280, 720, "raylib game");
    SetTargetFPS(60);

    // Load files relative to the .exe, so "assets/..." paths always work.
    ChangeDirectory(GetApplicationDirectory());

    while (!WindowShouldClose())
    {
        BeginDrawing();
        ClearBackground(RAYWHITE);
        EndDrawing();
    }

    CloseWindow();
    return 0;
}
