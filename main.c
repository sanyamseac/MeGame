#include "raylib.h"
#include "play.h"

int main(void)
{
    // Initialization
    const int screenWidth = 720;
    const int screenHeight = 720;

    InitWindow(screenWidth, screenHeight, "Main Menu");

    SetTargetFPS(60); // Set our game to run at 60 frames-per-second

    // Button properties
    Rectangle button = { screenWidth/2 - 100, screenHeight/2, 200, 50 };
    bool buttonHovered = false;

    // Main menu loop
    while (!WindowShouldClose()) // Detect window close button or ESC key
    {
        // Update
        Vector2 mousePoint = GetMousePosition();
        buttonHovered = CheckCollisionPointRec(mousePoint, button);

        if (buttonHovered && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
        {
            RunGame(); // Call the game function from play.c
        }

        // Draw
        BeginDrawing();

        ClearBackground(RAYWHITE);

        DrawText("Project MeGame", screenWidth/2 - MeasureText("Project MeGame", 40)/2, screenHeight/4, 40, DARKGRAY);

        if (buttonHovered)
        {
            DrawRectangleRec(button, LIGHTGRAY);
        }
        else
        {
            DrawRectangleRec(button, GRAY);
        }

        DrawText("Play", button.x + button.width/2 - MeasureText("Play", 20)/2, button.y + button.height/2 - 10, 20, DARKGRAY);

        EndDrawing();
    }

    // De-Initialization
    CloseWindow(); // Close window and OpenGL context

    return 0;
}