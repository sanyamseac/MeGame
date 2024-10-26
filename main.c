#include "raylib.h"
#include "play.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>

#define PORT 8080
#define MAX_CLIENTS 2

typedef struct {
    int socket;
    struct sockaddr_in address;
    int addr_len;
    int index;
} client_t;

client_t *clients[MAX_CLIENTS];
pthread_mutex_t clients_mutex = PTHREAD_MUTEX_INITIALIZER;

void broadcast(char *message, int exclude_index) {
    pthread_mutex_lock(&clients_mutex);
    for (int i = 0; i < MAX_CLIENTS; ++i) {
        if (clients[i] && clients[i]->index != exclude_index) {
            send(clients[i]->socket, message, strlen(message), 0);
        }
    }
    pthread_mutex_unlock(&clients_mutex);
}

void *handle_client(void *arg) {
    char buffer[1024];
    client_t *cli = (client_t *)arg;

    while (1) {
        int bytes_received = recv(cli->socket, buffer, sizeof(buffer), 0);
        if (bytes_received <= 0) {
            close(cli->socket);
            pthread_mutex_lock(&clients_mutex);
            clients[cli->index] = NULL;
            pthread_mutex_unlock(&clients_mutex);
            free(cli);
            break;
        } else {
            buffer[bytes_received] = '\0';
            broadcast(buffer, cli->index);
        }
    }

    return NULL;
}

void start_server() {
    int server_socket, new_socket;
    struct sockaddr_in server_addr, new_addr;
    socklen_t addr_size;
    pthread_t tid;

    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr));
    listen(server_socket, MAX_CLIENTS);

    printf("Server listening on port %d\n", PORT);

    while (1) {
        addr_size = sizeof(new_addr);
        new_socket = accept(server_socket, (struct sockaddr *)&new_addr, &addr_size);

        if (new_socket < 0) {
            perror("accept");
            exit(EXIT_FAILURE);
        }

        pthread_mutex_lock(&clients_mutex);
        for (int i = 0; i < MAX_CLIENTS; ++i) {
            if (!clients[i]) {
                client_t *cli = (client_t *)malloc(sizeof(client_t));
                cli->socket = new_socket;
                cli->address = new_addr;
                cli->addr_len = addr_size;
                cli->index = i;
                clients[i] = cli;
                pthread_create(&tid, NULL, handle_client, (void *)cli);
                break;
            }
        }
        pthread_mutex_unlock(&clients_mutex);
    }
}

int main(void) {
    // Start the server in a separate thread
    pthread_t server_thread;
    pthread_create(&server_thread, NULL, (void *)start_server, NULL);

    // Initialization
    const int screenWidth = 1920;
    const int screenHeight = 1080;
    
    InitWindow(screenWidth, screenHeight, "Main Menu");

    SetTargetFPS(60); // Set our game to run at 60 frames-per-second

    // Main menu loop
    while (!WindowShouldClose()) // Detect window close button or ESC key
    {
        // Update
        if (IsKeyPressed(KEY_ENTER)) 
        {
            CloseWindow();
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