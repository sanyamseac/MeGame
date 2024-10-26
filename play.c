#include "raylib.h"
#include <stdlib.h> // For rand() and srand()
#include <time.h>   // For time()

#define MAX_COINS 50

// Structure for coins
typedef struct Coin {
    Vector2 position;
    bool collected;
    Texture2D texture;
} Coin;

// Function declarations
void InitGame(int screenWidth, int screenHeight, int boundarySize, Vector2 *playerPosition, Camera2D *camera, Coin coins[], int *score, Texture2D *visibilityCircle, Texture2D candyTextures[], Texture2D playerTextures[]);
void UpdateGame(Vector2 *playerPosition, Vector2 *targetPosition, Camera2D *camera, int screenWidth, int screenHeight, int boundarySize, Coin coins[], int *score, Texture2D playerTextures[], Texture2D *currentPlayerTexture);
void DrawGame(Vector2 playerPosition, Camera2D camera, int boundarySize, Coin coins[], int score, int screenWidth, Texture2D visibilityCircle, Texture2D currentPlayerTexture);
void InitCoins(Coin coins[], int boundarySize, Texture2D candyTextures[]);
void UpdatePlayerPosition(Vector2 *playerPosition, Vector2 *targetPosition, Camera2D *camera, int screenWidth, int screenHeight, int boundarySize, Texture2D playerTextures[], Texture2D *currentPlayerTexture);
void CheckCoinCollisions(Vector2 playerPosition, Coin coins[], int *score);
void DrawBoundary(int boundarySize);
void DrawCoins(Coin coins[]);
void DrawPlayer(Vector2 playerPosition, Texture2D currentPlayerTexture);
void DrawPlayerCircle(Vector2 playerPosition, float radius, Texture2D visibilityCircle);

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
    Texture2D visibilityCircle;
    Texture2D candyTextures[3];
    Texture2D playerTextures[4];
    Texture2D currentPlayerTexture;

    InitGame(screenWidth, screenHeight, boundarySize, &playerPosition, &camera, coins, &score, &visibilityCircle, candyTextures, playerTextures);
    targetPosition = playerPosition;
    currentPlayerTexture = playerTextures[2]; // Initial player texture (standing still after moving left)

    SetTargetFPS(60); // Set our game to run at 60 frames-per-second

    // Main game loop
    while (!WindowShouldClose()) // Detect window close button or ESC key
    {
        UpdateGame(&playerPosition, &targetPosition, &camera, screenWidth, screenHeight, boundarySize, coins, &score, playerTextures, &currentPlayerTexture);
        DrawGame(playerPosition, camera, boundarySize, coins, score, screenWidth, visibilityCircle, currentPlayerTexture);
    }

    // De-Initialization
    UnloadTexture(visibilityCircle); // Unload texture
    for (int i = 0; i < 3; i++) {
        UnloadTexture(candyTextures[i]); // Unload candy textures
    }
    for (int i = 0; i < 4; i++) {
        UnloadTexture(playerTextures[i]); // Unload player textures
    }
    CloseWindow(); // Close window and OpenGL context
}

void InitGame(int screenWidth, int screenHeight, int boundarySize, Vector2 *playerPosition, Camera2D *camera, Coin coins[], int *score, Texture2D *visibilityCircle, Texture2D candyTextures[], Texture2D playerTextures[])
{
    InitWindow(screenWidth, screenHeight, "raylib [core] example - basic window");

    *playerPosition = (Vector2){ (float)screenWidth / 2, (float)screenHeight / 2 };
    camera->target = *playerPosition;
    camera->offset = (Vector2){ screenWidth / 2, screenHeight / 2 };
    camera->zoom = 1.0f;

    // Initialize random seed
    srand(time(NULL));

    // Load candy textures
    candyTextures[0] = LoadTexture("assets/Candy_1.png");
    candyTextures[1] = LoadTexture("assets/Candy_2.png");
    candyTextures[2] = LoadTexture("assets/Candy_3.png");

    // Load player textures
    playerTextures[0] = LoadTexture("assets/G_Left.png");
    playerTextures[1] = LoadTexture("assets/G_Right.png");
    playerTextures[2] = LoadTexture("assets/G_Still_after_left.png");
    playerTextures[3] = LoadTexture("assets/G_Still_after_right.png");

    // Initialize coins at random positions
    InitCoins(coins, boundarySize, candyTextures);

    *score = 0;

    // Load visibility circle texture
    *visibilityCircle = LoadTexture("assets/BlackVisibilityCircle.png");
}

void InitCoins(Coin coins[], int boundarySize, Texture2D candyTextures[])
{
    for (int i = 0; i < MAX_COINS; i++)
    {
        coins[i].position = (Vector2){ rand() % (boundarySize - 20) + 10, rand() % (boundarySize - 20) + 10 };
        coins[i].collected = false;
        coins[i].texture = candyTextures[rand() % 3]; // Randomly assign one of the candy textures
    }
}

void UpdateGame(Vector2 *playerPosition, Vector2 *targetPosition, Camera2D *camera, int screenWidth, int screenHeight, int boundarySize, Coin coins[], int *score, Texture2D playerTextures[], Texture2D *currentPlayerTexture)
{
    // Update player position
    UpdatePlayerPosition(playerPosition, targetPosition, camera, screenWidth, screenHeight, boundarySize, playerTextures, currentPlayerTexture);

    // Lerp camera to follow the player with some offset
    camera->target.x += (playerPosition->x - camera->target.x) * 0.1f;
    camera->target.y += (playerPosition->y - camera->target.y) * 0.1f;

    // Ensure camera doesn't move past the boundaries
    if (camera->target.x < screenWidth / 2) camera->target.x = screenWidth / 2;
    if (camera->target.y < screenHeight / 2) camera->target.y = screenHeight / 2;
    if (camera->target.x > boundarySize - screenWidth / 2) camera->target.x = boundarySize - screenWidth / 2;
    if (camera->target.y > boundarySize - screenHeight / 2) camera->target.y = boundarySize - screenHeight / 2;

    // Check for collisions with coins
    CheckCoinCollisions(*playerPosition, coins, score);
}

void UpdatePlayerPosition(Vector2 *playerPosition, Vector2 *targetPosition, Camera2D *camera, int screenWidth, int screenHeight, int boundarySize, Texture2D playerTextures[], Texture2D *currentPlayerTexture)
{
    static bool movingLeft = false;
    static bool movingRight = false;

    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
    {
        *targetPosition = GetMousePosition();
        targetPosition->x += camera->target.x - screenWidth / 2;
        targetPosition->y += camera->target.y - screenHeight / 2;
    }

    // Lerp player position towards target position
    float prevX = playerPosition->x;
    playerPosition->x += (targetPosition->x - playerPosition->x) * 0.1f;
    playerPosition->y += (targetPosition->y - playerPosition->y) * 0.1f;

    // Ensure player doesn't move past the boundaries
    if (playerPosition->x < 10) playerPosition->x = 10;
    if (playerPosition->y < 10) playerPosition->y = 10;
    if (playerPosition->x > boundarySize - 10) playerPosition->x = boundarySize - 10;
    if (playerPosition->y > boundarySize - 10) playerPosition->y = boundarySize - 10;

    // Update player texture based on movement
    if (playerPosition->x < prevX)
    {
        *currentPlayerTexture = playerTextures[0]; // Moving left
        movingLeft = true;
        movingRight = false;
    }
    else if (playerPosition->x > prevX)
    {
        *currentPlayerTexture = playerTextures[1]; // Moving right
        movingLeft = false;
        movingRight = true;
    }
    else
    {
        if (movingLeft)
        {
            *currentPlayerTexture = playerTextures[2]; // Standing still after moving left
        }
        else if (movingRight)
        {
            *currentPlayerTexture = playerTextures[3]; // Standing still after moving right
        }
    }
}

void CheckCoinCollisions(Vector2 playerPosition, Coin coins[], int *score)
{
    for (int i = 0; i < MAX_COINS; i++)
    {
        if (!coins[i].collected && CheckCollisionCircles(playerPosition, 20, (Vector2){coins[i].position.x + coins[i].texture.width / 20, coins[i].position.y + coins[i].texture.height / 20}, 1))
        {
            coins[i].collected = true;
            (*score)++;
        }
    }
}

void DrawGame(Vector2 playerPosition, Camera2D camera, int boundarySize, Coin coins[], int score, int screenWidth, Texture2D visibilityCircle, Texture2D currentPlayerTexture)
{
    BeginDrawing();
    ClearBackground(DARKGRAY);

    BeginMode2D(camera);

    // Draw boundary
    DrawBoundary(boundarySize);

    // Draw coins
    DrawCoins(coins);

    // Draw player character
    DrawPlayer(playerPosition, currentPlayerTexture);

    // Draw thin black circle around the player
    float radius = 15*(150 + (score * (screenWidth / 2 - 150) / MAX_COINS));
    DrawPlayerCircle(playerPosition, radius, visibilityCircle);

    EndMode2D();

    // Draw score
    DrawText(TextFormat("Score: %d", score), 10, 10, 20, WHITE);

    EndDrawing();
}

void DrawBoundary(int boundarySize)
{
    DrawRectangle(0, 0, boundarySize, 10, BLACK); // Top wall
    DrawRectangle(0, 0, 10, boundarySize, BLACK); // Left wall
    DrawRectangle(boundarySize - 10, 0, 10, boundarySize, BLACK); // Right wall
    DrawRectangle(0, boundarySize - 10, boundarySize, 10, BLACK); // Bottom wall
}

void DrawCoins(Coin coins[])
{
    for (int i = 0; i < MAX_COINS; i++)
    {
        if (!coins[i].collected)
        {
            DrawTextureEx(coins[i].texture, coins[i].position, 0.0f, 0.1f, WHITE); // Scale down by 0.1 (10 times smaller)
        }
    }
}

void DrawPlayer(Vector2 playerPosition, Texture2D currentPlayerTexture)
{
    DrawTextureEx(currentPlayerTexture, (Vector2){playerPosition.x - currentPlayerTexture.width * 0.1f / 2, playerPosition.y - currentPlayerTexture.height * 0.1f / 2}, 0.0f, 0.1f, WHITE); // Scale down by 0.1 (10 times smaller)
}

void DrawPlayerCircle(Vector2 playerPosition, float radius, Texture2D visibilityCircle)
{
    DrawTexturePro(
        visibilityCircle,
        (Rectangle){ 0, 0, visibilityCircle.width, visibilityCircle.height },
        (Rectangle){ playerPosition.x, playerPosition.y, radius * 2, radius * 2},
        (Vector2){ radius, radius },
        0.0f,
        WHITE
    );
}