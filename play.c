#include "raylib.h"
#include <stdlib.h>
#include <math.h>
#include <time.h>

#define MAX_COINS 50 // Define maximum number of coins
#define PLAYER_SPEED 0.025f // Define player speed for lerp
#define ROTATION_SPEED 0.06f // Define rotation speed for lerp

// Structure for coins
typedef struct Coin {
    Vector2 position;
    bool collected;
    Texture2D texture;
} Coin;

// Structure for player
typedef struct Player {
    Vector2 position;
    Vector2 targetPosition;
    Texture2D textures[4];
    Texture2D currentTexture;
    bool canCollectCoins;
    float rotation; // Rotation angle for visibility cone
    float targetRotation; // Target rotation angle for visibility cone
} Player;

// Function declarations
void InitGame(int screenWidth, int screenHeight, int boundarySize, Player *mainChar, Player *ghost, Camera2D *camera, Coin coins[], int *score, Texture2D *visibilityCircle, Texture2D *visibilityCone, Texture2D candyTextures[], Sound *coinSound, int playerChoice);
void UpdateGame(Player *player, Camera2D *camera, int screenWidth, int screenHeight, int boundarySize, Coin coins[], int *score, Sound coinSound);
void DrawGame(Player *player, Camera2D camera, int boundarySize, Coin coins[], int score, int screenWidth, int screenHeight, Texture2D visibilityCircle, Texture2D visibilityCone);
void InitCoins(Coin coins[], int boundarySize, Texture2D candyTextures[]);
void UpdatePlayerPosition(Player *player, Camera2D *camera, int screenWidth, int screenHeight, int boundarySize);
void CheckCoinCollisions(Player *player, Coin coins[], int *score, Sound coinSound);
void DrawBoundary(int boundarySize);
void DrawCoins(Coin coins[]);
void DrawPlayer(Player player);
void DrawPlayerCircle(Vector2 playerPosition, float radius, Texture2D visibilityCircle);
void DrawPlayerCone(Vector2 playerPosition, float rotation, Texture2D visibilityCone);
int ShowMenu(int screenWidth, int screenHeight);
void ShowLoadingScreen(void);

void RunGame(void)
{
    // Initialization
    const int screenWidth = 720;
    const int screenHeight = 720;
    const int boundarySize = 2000; // Size of the boundaries

    Player mainChar = { 0 };
    Player ghost = { 0 };
    Camera2D camera = { 0 };
    Coin coins[MAX_COINS] = { 0 };
    int score = 0;
    Texture2D visibilityCircle;
    Texture2D visibilityCone;
    Texture2D candyTextures[3];
    Sound coinSound;

    InitWindow(screenWidth, screenHeight, "Play");

    int playerChoice = ShowMenu(screenWidth, screenHeight);

    InitGame(screenWidth, screenHeight, boundarySize, &mainChar, &ghost, &camera, coins, &score, &visibilityCircle, &visibilityCone, candyTextures, &coinSound, playerChoice);

    Player *player = (playerChoice == 0) ? &mainChar : &ghost;

    SetTargetFPS(60); // Set our game to run at 60 frames-per-second

    // Main game loop
    while (!WindowShouldClose()) // Detect window close button or ESC key
    {
        UpdateGame(player, &camera, screenWidth, screenHeight, boundarySize, coins, &score, coinSound);
        DrawGame(player, camera, boundarySize, coins, score, screenWidth, screenHeight, visibilityCircle, visibilityCone);
    }

    // De-Initialization
    UnloadTexture(visibilityCircle); // Unload texture
    UnloadTexture(visibilityCone); // Unload cone texture
    for (int i = 0; i < 3; i++) {
        UnloadTexture(candyTextures[i]); // Unload candy textures
    }
    for (int i = 0; i < 4; i++) {
        UnloadTexture(mainChar.textures[i]); // Unload main character textures
        UnloadTexture(ghost.textures[i]); // Unload ghost textures
    }
    UnloadSound(coinSound); // Unload sound
    CloseAudioDevice(); // Close audio device
    CloseWindow(); // Close window and OpenGL context
}

void ShowLoadingScreen(void)
{
    BeginDrawing();
    ClearBackground(RAYWHITE);
    DrawText("Loading...", 350, 360, 20, DARKGRAY);
    EndDrawing();
}

int ShowMenu(int screenWidth, int screenHeight)
{
    int choice = -1;
    Rectangle mainCharButton = { screenWidth / 2 - 100, screenHeight / 2, 200, 50 };
    Rectangle ghostButton = { screenWidth / 2 - 100, screenHeight / 2 + 60, 200, 50 };

    while (choice == -1 && !WindowShouldClose())
    {
        BeginDrawing();
        ClearBackground(RAYWHITE);

        DrawText("Choose your character:", screenWidth / 2 - MeasureText("Choose your character:", 20) / 2, screenHeight / 2 - 50, 20, DARKGRAY);

        if (CheckCollisionPointRec(GetMousePosition(), mainCharButton))
        {
            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) choice = 0;
            DrawRectangleRec(mainCharButton, LIGHTGRAY);
        }
        else
        {
            DrawRectangleRec(mainCharButton, GRAY);
        }
        DrawText("Main Character", mainCharButton.x + 20, mainCharButton.y + 15, 20, DARKGRAY);

        if (CheckCollisionPointRec(GetMousePosition(), ghostButton))
        {
            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) choice = 1;
            DrawRectangleRec(ghostButton, LIGHTGRAY);
        }
        else
        {
            DrawRectangleRec(ghostButton, GRAY);
        }
        DrawText("Ghost", ghostButton.x + 70, ghostButton.y + 15, 20, DARKGRAY);

        EndDrawing();
    }

    return choice;
}

void InitGame(int screenWidth, int screenHeight, int boundarySize, Player *mainChar, Player *ghost, Camera2D *camera, Coin coins[], int *score, Texture2D *visibilityCircle, Texture2D *visibilityCone, Texture2D candyTextures[], Sound *coinSound, int playerChoice)
{
    ShowLoadingScreen();

    InitAudioDevice(); // Initialize audio device
    SetMasterVolume(1.0f); // Set the master volume to maximum

    mainChar->position = (Vector2){ (float)screenWidth / 2, (float)screenHeight / 2 };
    ghost->position = (Vector2){ (float)screenWidth / 2 + 50, (float)screenHeight / 2 + 50 };
    camera->target = mainChar->position;
    camera->offset = (Vector2){ screenWidth / 2, screenHeight / 2 };
    camera->zoom = 1.0f;

    // Initialize random seed
    srand(time(NULL));

    // Load candy textures
    candyTextures[0] = LoadTexture("assets/Props/Candy_1.png");
    candyTextures[1] = LoadTexture("assets/Props/Candy_2.png");
    candyTextures[2] = LoadTexture("assets/Props/Candy_3.png");

    // Load main character textures
    mainChar->textures[0] = LoadTexture("assets/Characters/Main_Char_Left.png");
    mainChar->textures[1] = LoadTexture("assets/Characters/Main_Char_Right.png");
    mainChar->textures[2] = LoadTexture("assets/Characters/Main_Char_Still_after_Left.png");
    mainChar->textures[3] = LoadTexture("assets/Characters/Main_Char_Still_after_Right.png");
    mainChar->currentTexture = mainChar->textures[2]; // Initial texture
    mainChar->canCollectCoins = true;
    mainChar->rotation = 0.0f;
    mainChar->targetRotation = 0.0f;

    // Load ghost textures
    ghost->textures[0] = LoadTexture("assets/Characters/G_Left.png");
    ghost->textures[1] = LoadTexture("assets/Characters/G_Right.png");
    ghost->textures[2] = LoadTexture("assets/Characters/G_Still_after_left.png");
    ghost->textures[3] = LoadTexture("assets/Characters/G_Still_after_right.png");
    ghost->currentTexture = ghost->textures[2]; // Initial texture
    ghost->canCollectCoins = false;
    ghost->rotation = 0.0f;
    ghost->targetRotation = 0.0f;

    // Initialize coins at random positions
    InitCoins(coins, boundarySize, candyTextures);

    *score = 0;

    // Load visibility circle texture
    *visibilityCircle = LoadTexture("assets/Others/BlackVisibilityCircle.png");

    // Load visibility cone texture
    *visibilityCone = LoadTexture("assets/Others/BlackVisibilityCone.png");

    // Load coin sound
    *coinSound = LoadSound("assets/audio/game-bonus.mp3");

    // Set initial target positions
    mainChar->targetPosition = mainChar->position;
    ghost->targetPosition = ghost->position;

    // If the player chooses the ghost, update the camera target to the ghost
    if (playerChoice == 1) {
        camera->target = ghost->position;
    }
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

void CheckGameEnd(Player *player, int score, int screenWidth, int screenHeight)
{
    if (score >= MAX_COINS)
    {
        BeginDrawing();
        ClearBackground(RAYWHITE);

        if (player->canCollectCoins)
        {
            DrawText("Victory! Main Character Wins!", screenWidth / 2 - MeasureText("Victory! Main Character Wins!", 20) / 2, screenHeight / 2, 20, DARKGRAY);
        }
        else
        {
            DrawText("Game Over! Ghost Loses!", screenWidth / 2 - MeasureText("Game Over! Ghost Loses!", 20) / 2, screenHeight / 2, 20, DARKGRAY);
        }

        EndDrawing();

        // Wait for a few seconds before closing the window
        WaitTime(7.0f);
        CloseWindow();
    }
}

void UpdateGame(Player *player, Camera2D *camera, int screenWidth, int screenHeight, int boundarySize, Coin coins[], int *score, Sound coinSound)
{
    // Update player position
    UpdatePlayerPosition(player, camera, screenWidth, screenHeight, boundarySize);

    // Lerp camera to follow the player with some offset
    camera->target.x += (player->position.x - camera->target.x) * 0.1f;
    camera->target.y += (player->position.y - camera->target.y) * 0.1f;

    // Ensure camera doesn't move past the boundaries
    if (camera->target.x < screenWidth / 2) camera->target.x = screenWidth / 2;
    if (camera->target.y < screenHeight / 2) camera->target.y = screenHeight / 2;
    if (camera->target.x > boundarySize - screenWidth / 2) camera->target.x = boundarySize - screenWidth / 2;
    if (camera->target.y > boundarySize - screenHeight / 2) camera->target.y = boundarySize - screenHeight / 2;

    // Check for collisions with coins for the player
    CheckCoinCollisions(player, coins, score, coinSound);

    // Check if the game has ended
    CheckGameEnd(player, *score, screenWidth, screenHeight);
}

void UpdatePlayerPosition(Player *player, Camera2D *camera, int screenWidth, int screenHeight, int boundarySize)
{
    static bool movingLeft = false;
    static bool movingRight = false;

    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
    {
        player->targetPosition = GetMousePosition();
        player->targetPosition.x += camera->target.x - screenWidth / 2;
        player->targetPosition.y += camera->target.y - screenHeight / 2;

        // Calculate the angle between the current position and the target position
        Vector2 direction = { player->targetPosition.x - player->position.x, player->targetPosition.y - player->position.y };
        player->targetRotation = atan2f(direction.y, direction.x) * (180.0f / PI);
    }
    static Sound walkingSound = { 0 };
    static bool isWalkingSoundPlaying = false;

    if (walkingSound.frameCount == 0) {
        walkingSound = LoadSound("assets/audio/strange-notification-36458.mp3");
    }

    // Lerp player position towards target position
    float prevX = player->position.x;
    player->position.x += (player->targetPosition.x - player->position.x) * PLAYER_SPEED;
    player->position.y += (player->targetPosition.y - player->position.y) * PLAYER_SPEED;

    // Ensure player doesn't move past the boundaries
    if (player->position.x < 10) player->position.x = 10;
    if (player->position.y < 10) player->position.y = 10;
    if (player->position.x > boundarySize - 10) player->position.x = boundarySize - 10;
    if (player->position.y > boundarySize - 10) player->position.y = boundarySize - 10;

    // Update player texture based on movement
    if ((player->position.x - prevX < -2))
    {
        player->currentTexture = player->textures[0]; // Moving left
        movingLeft = true;
        movingRight = false;
        if (!isWalkingSoundPlaying) {
            PlaySound(walkingSound);
            isWalkingSoundPlaying = true;
        }
    }
    else if ((player->position.x - prevX > 2))
    {
        player->currentTexture = player->textures[1]; // Moving right
        movingLeft = false;
        movingRight = true;
        if (!isWalkingSoundPlaying) {
            PlaySound(walkingSound);
            isWalkingSoundPlaying = true;
        }
    }
    else
    {
        if (movingLeft)
        {
            player->currentTexture = player->textures[2]; // Standing still after moving left
        }
        else if (movingRight)
        {
            player->currentTexture = player->textures[3]; // Standing still after moving right
        }
        if (isWalkingSoundPlaying) {
            StopSound(walkingSound);
            isWalkingSoundPlaying = false;
        }
    }

    // Lerp rotation towards target rotation
    player->rotation += (player->targetRotation - player->rotation) * ROTATION_SPEED;
}

void CheckCoinCollisions(Player *player, Coin coins[], int *score, Sound coinSound)
{
    if (!player->canCollectCoins) return;

    for (int i = 0; i < MAX_COINS; i++)
    {
        if (!coins[i].collected && CheckCollisionCircles(player->position, 20, (Vector2){coins[i].position.x + coins[i].texture.width / 20, coins[i].position.y + coins[i].texture.height / 20}, 1))
        {
            coins[i].collected = true;
            (*score)++;
            PlaySound(coinSound); // Play sound when a coin is collected
        }
    }
}

void DrawGame(Player *player, Camera2D camera, int boundarySize, Coin coins[], int score, int screenWidth, int screenHeight, Texture2D visibilityCircle, Texture2D visibilityCone)
{
    static Texture2D background = { 0 };
    if (background.id == 0) {
        background = LoadTexture("assets/Others/BackgroundTile.png");
    }

    BeginDrawing();
    ClearBackground(RAYWHITE);

    BeginMode2D(camera);

    // Draw background tiles fixed to the world map
    for (int y = 0; y < boundarySize; y += background.height) {
        for (int x = 0; x < boundarySize; x += background.width) {
            DrawTexture(background, x, y, WHITE);
        }
    }

    // Draw boundary
    DrawBoundary(boundarySize);

    // Draw coins
    DrawCoins(coins);

    // Draw player
    DrawPlayer(*player);

    // Draw visibility circle or cone based on player type
    if (player->canCollectCoins) {
        float radius = 15 * (150 + (score * (screenWidth / 2 - 150) / MAX_COINS));
        DrawPlayerCircle(player->position, radius, visibilityCircle);
    } else {
        DrawPlayerCone(player->position, player->rotation, visibilityCone);
    }

    EndMode2D();

    // Draw score
    static Texture2D candyIcon = { 0 };
    if (candyIcon.id == 0) {
        candyIcon = LoadTexture("assets/Props/Candy_3.png");
    }
    DrawTextureEx(candyIcon, (Vector2){37, 30}, 180.0f, 0.1f, WHITE); // Scale down by 0.1 (10 times smaller)
    DrawText(TextFormat("Candies Left: %d", MAX_COINS - score), 40, 10, 20, WHITE);

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
    static Texture2D glow = { 0 };
    if (glow.id == 0) {
        glow = LoadTexture("assets/Props/glow.png");
    }

    for (int i = 0; i < MAX_COINS; i++)
    {
        if (!coins[i].collected)
        {
            // Draw glow behind the coin
            DrawTextureEx(glow, (Vector2){coins[i].position.x - glow.width * 0.05f / 2 + (rand() % 4 + 12), coins[i].position.y - glow.height * 0.05f / 2 + (rand() % 4 + 12)}, 0.0f, 0.05f, WHITE); // Scale down by 0.05 (20 times smaller)
            // Draw the coin
            DrawTextureEx(coins[i].texture, coins[i].position, 0.0f, 0.1f, WHITE); // Scale down by 0.1 (10 times smaller)
        }
    }
}

void DrawPlayer(Player player)
{
    DrawTextureEx(player.currentTexture, (Vector2){player.position.x - player.currentTexture.width * 0.1f / 2, player.position.y - player.currentTexture.height * 0.1f / 2}, 0.0f, 0.1f, WHITE); // Scale down by 0.1 (10 times smaller)
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

void DrawPlayerCone(Vector2 playerPosition, float rotation, Texture2D visibilityCone)
{
    DrawTexturePro(
        visibilityCone,
        (Rectangle){ 0, 0, visibilityCone.width, visibilityCone.height },
        (Rectangle){ playerPosition.x, playerPosition.y, visibilityCone.width, visibilityCone.height},
        (Vector2){ visibilityCone.width / 2, visibilityCone.height / 2 },
        rotation,
        WHITE
    );
}