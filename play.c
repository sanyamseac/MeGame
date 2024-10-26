#include "raylib.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>

<<<<<<< Updated upstream
#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 8080
#define MAX_PLAYERS 10
=======
#define MAX_COINS 50
>>>>>>> Stashed changes

typedef struct Player {
    Vector2 position;
<<<<<<< Updated upstream
    Color color;
    int upKey;
    int downKey;
    int leftKey;
    int rightKey;
} Player;

int client_socket;
Player localPlayer;
Player remotePlayers[MAX_PLAYERS];
int numRemotePlayers = 0;
=======
    bool collected;
    Texture2D texture;
} Coin;

// Function declarations
void InitGame(int screenWidth, int screenHeight, int boundarySize, Vector2 *playerPosition, Camera2D *camera, Coin coins[], int *score, Texture2D *visibilityCircle, Texture2D candyTextures[]);
void UpdateGame(Vector2 *playerPosition, Vector2 *targetPosition, Camera2D *camera, int screenWidth, int screenHeight, int boundarySize, Coin coins[], int *score);
void DrawGame(Vector2 playerPosition, Camera2D camera, int boundarySize, Coin coins[], int score, int screenWidth, Texture2D visibilityCircle);
void InitCoins(Coin coins[], int boundarySize, Texture2D candyTextures[]);
void UpdatePlayerPosition(Vector2 *playerPosition, Vector2 *targetPosition, Camera2D *camera, int screenWidth, int screenHeight, int boundarySize);
void CheckCoinCollisions(Vector2 playerPosition, Coin coins[], int *score);
void DrawBoundary(int boundarySize);
void DrawCoins(Coin coins[]);
void DrawPlayerCircle(Vector2 playerPosition, float radius, Texture2D visibilityCircle);
>>>>>>> Stashed changes

void *receive_data(void *arg) {
    char buffer[1024];
    while (1) {
        int bytes_received = recv(client_socket, buffer, sizeof(buffer), 0);
        if (bytes_received > 0) {
            buffer[bytes_received] = '\0';
            int playerIndex;
            float posX, posY;
            sscanf(buffer, "%d %f %f", &playerIndex, &posX, &posY);
            if (playerIndex >= 0 && playerIndex < MAX_PLAYERS) {
                remotePlayers[playerIndex].position.x = posX;
                remotePlayers[playerIndex].position.y = posY;
            }
        }
    }
    return NULL;
}

void RunGame(void) {
    // Initialization
    const int screenWidth = 1920;
    const int screenHeight = 1080;

<<<<<<< Updated upstream
    InitWindow(screenWidth, screenHeight, "raylib [core] example - multiplayer");

    localPlayer = (Player){{(float)screenWidth / 4, (float)screenHeight / 2}, RED, KEY_W, KEY_S, KEY_A, KEY_D};
    for (int i = 0; i < MAX_PLAYERS; i++) {
        remotePlayers[i] = (Player){{0, 0}, BLUE, 0, 0, 0, 0};
    }
=======
    Vector2 playerPosition = { 0 };
    Vector2 targetPosition = { 0 };
    Camera2D camera = { 0 };
    Coin coins[MAX_COINS] = { 0 };
    int score = 0;
    Texture2D visibilityCircle;
    Texture2D candyTextures[3];

    InitGame(screenWidth, screenHeight, boundarySize, &playerPosition, &camera, coins, &score, &visibilityCircle, candyTextures);
    targetPosition = playerPosition;
>>>>>>> Stashed changes

    SetTargetFPS(60); // Set our game to run at 60 frames-per-second

    // Connect to server
    struct sockaddr_in server_addr;
    client_socket = socket(AF_INET, SOCK_STREAM, 0);
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);
    inet_pton(AF_INET, SERVER_IP, &server_addr.sin_addr);

    connect(client_socket, (struct sockaddr *)&server_addr, sizeof(server_addr));

    pthread_t receive_thread;
    pthread_create(&receive_thread, NULL, receive_data, NULL);

    // Main game loop
    while (!WindowShouldClose()) // Detect window close button or ESC key
    {
<<<<<<< Updated upstream
        // Update
        if (IsKeyDown(localPlayer.rightKey)) localPlayer.position.x += 2.0f;
        if (IsKeyDown(localPlayer.leftKey)) localPlayer.position.x -= 2.0f;
        if (IsKeyDown(localPlayer.upKey)) localPlayer.position.y -= 2.0f;
        if (IsKeyDown(localPlayer.downKey)) localPlayer.position.y += 2.0f;

        // Send player position to server
        char buffer[1024];
        sprintf(buffer, "%d %f %f", 0, localPlayer.position.x, localPlayer.position.y); // Assuming local player is player 0
        send(client_socket, buffer, strlen(buffer), 0);

        // Draw
        BeginDrawing();

        ClearBackground(DARKGRAY);

        DrawCircleV(localPlayer.position, 20, localPlayer.color); // Draw local player
        for (int i = 0; i < MAX_PLAYERS; i++) {
            if (remotePlayers[i].position.x != 0 || remotePlayers[i].position.y != 0) {
                DrawCircleV(remotePlayers[i].position, 20, remotePlayers[i].color); // Draw remote players
            }
        }

        EndDrawing();
=======
        UpdateGame(&playerPosition, &targetPosition, &camera, screenWidth, screenHeight, boundarySize, coins, &score);
        DrawGame(playerPosition, camera, boundarySize, coins, score, screenWidth, visibilityCircle);
>>>>>>> Stashed changes
    }

    // De-Initialization
    UnloadTexture(visibilityCircle); // Unload texture
    for (int i = 0; i < 3; i++) {
        UnloadTexture(candyTextures[i]); // Unload candy textures
    }
    CloseWindow(); // Close window and OpenGL context
<<<<<<< Updated upstream
    close(client_socket);
=======
}

void InitGame(int screenWidth, int screenHeight, int boundarySize, Vector2 *playerPosition, Camera2D *camera, Coin coins[], int *score, Texture2D *visibilityCircle, Texture2D candyTextures[])
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

void UpdateGame(Vector2 *playerPosition, Vector2 *targetPosition, Camera2D *camera, int screenWidth, int screenHeight, int boundarySize, Coin coins[], int *score)
{
    // Update player position
    UpdatePlayerPosition(playerPosition, targetPosition, camera, screenWidth, screenHeight, boundarySize);

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

void UpdatePlayerPosition(Vector2 *playerPosition, Vector2 *targetPosition, Camera2D *camera, int screenWidth, int screenHeight, int boundarySize)
{
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

void DrawGame(Vector2 playerPosition, Camera2D camera, int boundarySize, Coin coins[], int score, int screenWidth, Texture2D visibilityCircle)
{
    BeginDrawing();
    ClearBackground(DARKGRAY);

    BeginMode2D(camera);

    // Draw boundary
    DrawBoundary(boundarySize);

    // Draw coins
    DrawCoins(coins);

    DrawCircleV(playerPosition, 20, RED); // Draw player character as a red circle

    // Draw thin black circle around the player
    float radius = 15*(150 + (score * (screenWidth / 2 - 150) / MAX_COINS));
    DrawPlayerCircle(playerPosition, radius, visibilityCircle);

    EndMode2D();

    // Draw score
    DrawText(TextFormat("Score: %d", score), 10, 10, 20, WHITE);

    EndDrawing();
>>>>>>> Stashed changes
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