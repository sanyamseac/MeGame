#include "raylib.h"
#include "play.h"

int main(void)
{
    // Initialization
    const int screenWidth = 720;
    const int screenHeight = 720;

    InitWindow(screenWidth, screenHeight, "Main Menu");

    SetTargetFPS(60); // Set our game to run at 60 frames-per-second

    // Main menu loop
    while (!WindowShouldClose()) // Detect window close button or ESC key
    {
        // Update
        if (IsKeyPressed(KEY_ENTER)) 
        {
            RunGame(); // Call the game function from play.c
        }

        // Draw
        BeginDrawing();

        ClearBackground(RAYWHITE);

        DrawText("Press ENTER to Play", screenWidth/2 - MeasureText("Press ENTER to Play", 20)/2, screenHeight/2 - 10, 20, LIGHTGRAY);

        EndDrawing();
    }

    // De-Initialization
    CloseWindow(); // Close window and OpenGL context

    return 0;
}