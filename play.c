#include "raylib.h"
#include <stdlib.h> // For rand() and srand()
#include <time.h>   // For time()

#define MAX_COINS 10

// Structure for coins
typedef struct Coin {
    Vector2 position;
    bool collected;
} Coin;

// Function declarations
void InitGame(int screenWidth, int screenHeight, int boundarySize, Vector2 *playerPosition, Camera2D *camera, Coin coins[], int *score);
void UpdateGame(Vector2 *playerPosition, Vector2 *targetPosition, Camera2D *camera, int screenWidth, int screenHeight, int boundarySize, Coin coins[], int *score);
void DrawGame(Vector2 playerPosition, Camera2D camera, int boundarySize, Coin coins[], int score);

void RunGame(void)
{
    // Initialization
    const int screenWidth = 720;
    const int screenHeight = 720;
    const int boundarySize = 2000; // Size of the boundaries

    Vector2 playerPosition = { 0 };
    Vector2 targetPosition = { 0 };
    Camera2D camera = { 0 };
    Coin coins[MAX_COINS] = { 0 };
    int score = 0;

    InitGame(screenWidth, screenHeight, boundarySize, &playerPosition, &camera, coins, &score);
    targetPosition = playerPosition;

    SetTargetFPS(60); // Set our game to run at 60 frames-per-second

    // Main game loop
    while (!WindowShouldClose()) // Detect window close button or ESC key
    {
        UpdateGame(&playerPosition, &targetPosition, &camera, screenWidth, screenHeight, boundarySize, coins, &score);
        DrawGame(playerPosition, camera, boundarySize, coins, score);
    }

    // De-Initialization
    CloseWindow(); // Close window and OpenGL context
}

void InitGame(int screenWidth, int screenHeight, int boundarySize, Vector2 *playerPosition, Camera2D *camera, Coin coins[], int *score)
{
    InitWindow(screenWidth, screenHeight, "raylib [core] example - basic window");

    *playerPosition = (Vector2){ (float)screenWidth / 2, (float)screenHeight / 2 };
    camera->target = *playerPosition;
    camera->offset = (Vector2){ screenWidth / 2, screenHeight / 2 };
    camera->zoom = 1.0f;

    // Initialize random seed
    srand(time(NULL));

    // Initialize coins at random positions
    for (int i = 0; i < MAX_COINS; i++)
    {
        coins[i].position = (Vector2){ rand() % (boundarySize - 20) + 10, rand() % (boundarySize - 20) + 10 };
        coins[i].collected = false;
    }

    *score = 0;
}

void UpdateGame(Vector2 *playerPosition, Vector2 *targetPosition, Camera2D *camera, int screenWidth, int screenHeight, int boundarySize, Coin coins[], int *score)
{
    // Update
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
    {
        *targetPosition = GetMousePosition();
        targetPosition->x += camera->target.x - screenWidth / 2;
        targetPosition->y += camera->target.y - screenHeight / 2;
    }

    // Lerp player position towards target position
    playerPosition->x += (targetPosition->x - playerPosition->x) * 0.1f;
    playerPosition->y += (targetPosition->y - playerPosition->y) * 0.1f;

    // Ensure player doesn't move past the boundaries
    if (playerPosition->x < 10) playerPosition->x = 10;
    if (playerPosition->y < 10) playerPosition->y = 10;
    if (playerPosition->x > boundarySize - 10) playerPosition->x = boundarySize - 10;
    if (playerPosition->y > boundarySize - 10) playerPosition->y = boundarySize - 10;

    // Lerp camera to follow the player with some offset
    camera->target.x += (playerPosition->x - camera->target.x) * 0.1f;
    camera->target.y += (playerPosition->y - camera->target.y) * 0.1f;

    // Check for collisions with coins
    for (int i = 0; i < MAX_COINS; i++)
    {
        if (!coins[i].collected && CheckCollisionCircles(*playerPosition, 20, coins[i].position, 10))
        {
            coins[i].collected = true;
            (*score)++;
        }
    }
}

void DrawGame(Vector2 playerPosition, Camera2D camera, int boundarySize, Coin coins[], int score)
{
    BeginDrawing();
    ClearBackground(DARKGRAY);

    BeginMode2D(camera);

    // Draw black walls around the edge of the boundary
    DrawRectangle(0, 0, boundarySize, 10, BLACK); // Top wall
    DrawRectangle(0, 0, 10, boundarySize, BLACK); // Left wall
    DrawRectangle(boundarySize - 10, 0, 10, boundarySize, BLACK); // Right wall
    DrawRectangle(0, boundarySize - 10, boundarySize, 10, BLACK); // Bottom wall

    // Draw coins
    for (int i = 0; i < MAX_COINS; i++)
    {
        if (!coins[i].collected)
        {
            DrawCircleV(coins[i].position, 10, GOLD);
        }
    }

    DrawCircleV(playerPosition, 20, RED); // Draw player character as a red circle

    EndMode2D();

    // Draw score
    DrawText(TextFormat("Score: %d", score), 10, 10, 20, WHITE);

    EndDrawing();
}
