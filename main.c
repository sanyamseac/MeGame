#include "raylib.h"

int main(void)
{
    // Initialization
    const int screenWidth = 800;
    const int screenHeight = 450;

    InitWindow(screenWidth, screenHeight, "raylib Starter Window");

    SetTargetFPS(60); // Set FPS to 60 (frames per second)

    // Main game loop
    while (!WindowShouldClose()) // Detect window close button or ESC key
    {
        // Update
        // Here, you can update variables, check for input, etc.

        // Draw
        BeginDrawing();

        ClearBackground(RAYWHITE);

        DrawText("Hello, raylib!", 190, 200, 20, LIGHTGRAY);

        EndDrawing();
    }

    // De-Initialization
    CloseWindow(); // Close window and OpenGL context

    return 0;
}