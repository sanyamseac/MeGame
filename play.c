#include "raylib.h"

void RunGame(void)
{
    // Initialization
    const int screenWidth = 720;
    const int screenHeight = 720;

    InitWindow(screenWidth, screenHeight, "raylib [core] example - basic window");

    Vector2 playerPosition = { (float)screenWidth/2, (float)screenHeight/2 };
    SetTargetFPS(60); // Set our game to run at 60 frames-per-second

    // Main game loop
    while (!WindowShouldClose()) // Detect window close button or ESC key
    {
        // Update
        if (IsKeyDown(KEY_RIGHT)) playerPosition.x += 2.0f;
        if (IsKeyDown(KEY_LEFT)) playerPosition.x -= 2.0f;
        if (IsKeyDown(KEY_UP)) playerPosition.y -= 2.0f;
        if (IsKeyDown(KEY_DOWN)) playerPosition.y += 2.0f;

        // Draw
        BeginDrawing();

        ClearBackground(DARKGRAY);

        DrawCircleV(playerPosition, 20, RED); // Draw player character as a red circle

        EndDrawing();
    }

    // De-Initialization
    CloseWindow(); // Close window and OpenGL context
}