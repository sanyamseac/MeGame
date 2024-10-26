#include "raylib.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>

#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 8080
#define MAX_PLAYERS 10

typedef struct Player {
    Vector2 position;
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

    InitWindow(screenWidth, screenHeight, "raylib [core] example - multiplayer");

    localPlayer = (Player){{(float)screenWidth / 4, (float)screenHeight / 2}, RED, KEY_W, KEY_S, KEY_A, KEY_D};
    for (int i = 0; i < MAX_PLAYERS; i++) {
        remotePlayers[i] = (Player){{0, 0}, BLUE, 0, 0, 0, 0};
    }

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
    }

    // De-Initialization
    CloseWindow(); // Close window and OpenGL context
    close(client_socket);
}
