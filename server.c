#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <ifaddrs.h>
#include <netinet/in.h>

#define PORT 8000 
#define BUFFER_SIZE 1024

// Data store (simple array for demonstration)
#define MAX_CLIENTS 10

int connectedClients = 0;
int roomSize = 0;
char *data_store[MAX_CLIENTS];

// Function prototypes
void handle_client(int client_sock);
void handle_create(int client_sock, char *data);
void handle_read(int client_sock, int index);
void handle_update(int client_sock, int index, char *data);
void handle_delete(int client_sock, int index);
void get_clients_count(int client_sock);

int get_local_ip(char *ip_buffer, size_t buffer_size);

const char *ip_to_string(struct in_addr ip_addr) {
    return inet_ntoa(ip_addr);  // Converts to dotted-decimal string format
}

int main() {
    int server_sock, client_sock;
    struct sockaddr_in server_addr, client_addr;
    int addr_len = sizeof(client_addr);

    // Initialize server socket
    if ((server_sock = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Socket creation error");
        exit(EXIT_FAILURE);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    // Bind socket to PORT
    if (bind(server_sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Socket bind failed");
        close(server_sock);
        exit(EXIT_FAILURE);
    }

    // Listen for incoming connections
    if (listen(server_sock, 10) < 0) { // queue upto 10 requests before rejecting.
        perror("Socket listen failed");
        close(server_sock);
        exit(EXIT_FAILURE);
    }
    char ipaddr[256];
    get_local_ip(ipaddr, 256);
    printf("CRUD server listening on port %s:%d\n", ipaddr, PORT);

    // Accept and handle incoming client connections
    while (1) {  // Keep accepting connections indefinitely
        if(connectedClients == 0) roomSize = 0;
        client_sock = accept(server_sock, (struct sockaddr *)&client_addr, (socklen_t *)&addr_len);
        if (client_sock < 0) {
            perror("Socket accept failed");
            continue; // Try to accept the next connection
        }
        // connectedClients++;
        printf("Accepted new connection from IP: %s\n", ip_to_string(client_addr.sin_addr));
        printf("Connected: %d\n", connectedClients);
        handle_client(client_sock);
        // connectedClients--;
        for(int i = 0; i < MAX_CLIENTS; i++){
            printf("%s ", data_store[i]);
        }
        printf("\n");
        // printf("Client ID: %d disconnected.\n", client_sock);
        close(client_sock);  // Close the client socket after handling the request
    }

    close(server_sock);
    return 0;
}

// Function to handle CRUD commands from the client
void handle_client(int client_sock) {
    char buffer[BUFFER_SIZE] = {0};  // Initialize buffer with zeroes

    // Clear buffer and read from the client
    int bytes_read = recv(client_sock, buffer, BUFFER_SIZE - 1, 0); // Leave space for null terminator
    if (bytes_read < 0) {
        perror("Error receiving data");
        close(client_sock);
        return;
    }
    buffer[bytes_read] = '\0';  // Explicitly null-terminate the buffer

    printf("[DEBUG] Received command: %s\n", buffer);

    // Parsing the command and arguments
    char command[BUFFER_SIZE] = {0}, data[BUFFER_SIZE] = {0};
    int index = -1;

    // Compare against supported commands
    if (strncmp(buffer, "CREATE", 6) == 0) {
        sscanf(buffer, "CREATE %s", data);
        handle_create(client_sock, data);
    } else if (strncmp(buffer, "READ", 4) == 0) {
        sscanf(buffer, "READ %d", &index);
        handle_read(client_sock, index);
    } else if (strncmp(buffer, "UPDATE", 6) == 0) {
        sscanf(buffer, "UPDATE %d %s", &index, data);
        handle_update(client_sock, index, data);
    } else if (strncmp(buffer, "DELETE", 6) == 0) {
        sscanf(buffer, "DELETE %d", &index);
        handle_delete(client_sock, index);
    } else if (strncmp(buffer, "COUNT", 5) == 0) {
        get_clients_count(client_sock);
    } else {
        const char *error_msg = "-1";
        send(client_sock, error_msg, strlen(error_msg), 0);
    }

    printf("[DEBUG] Completed command processing.\n");
}



// CREATE operation: store data in the first available slot and return that id to client.
void handle_create(int client_sock, char *data) {
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (data_store[i] == NULL) {
            data_store[i] = strdup(data);
            if (data_store[i] == NULL) {
                perror("Memory allocation error");
                char *message = "-1\n";
                send(client_sock, message, strlen(message), 0);
                return;
            }
            char response[BUFFER_SIZE];
            connectedClients++;
            snprintf(response, BUFFER_SIZE, "%d", i);
            send(client_sock, response, strlen(response), 0);
            return;
        }
    }
    char *message = "-1";
    send(client_sock, message, strlen(message), 0);
}

// READ operation: send data at the specified index to the client
void handle_read(int client_sock, int index) {
    if (index < 0 || index >= MAX_CLIENTS || data_store[index] == NULL) {
        char *message = "-1";
        send(client_sock, message, strlen(message), 0);
    } else {
        send(client_sock, data_store[index], strlen(data_store[index]), 0);
    }
}

// UPDATE operation: update data at the specified index
void handle_update(int client_sock, int index, char *data) {
    if (index < 0 || index >= MAX_CLIENTS || data_store[index] == NULL) {
        char *message = "-1";
        send(client_sock, message, strlen(message), 0);
    } else {
        free(data_store[index]);
        data_store[index] = strdup(data);
        if (data_store[index] == NULL) {
            perror("Memory allocation error");
            char *message = "-1";
            send(client_sock, message, strlen(message), 0);
            return;
        }
        char *message = "0";
        send(client_sock, message, strlen(message), 0);
    }
}

// DELETE operation: delete data at the specified index
void handle_delete(int client_sock, int index) {
    if (index < 0 || index >= MAX_CLIENTS || data_store[index] == NULL) {
        char *message = "-1";
        send(client_sock, message, strlen(message), 0);
    } else {
        free(data_store[index]);
        data_store[index] = NULL;
        char *message = "0";
        connectedClients--;
        send(client_sock, message, strlen(message), 0);
    }
}

// GET the local ip address on which server is serving to display.
int get_local_ip(char *ip_buffer, size_t buffer_size) {
    struct ifaddrs *ifaddr, *ifa;
    int family;
    int found = 0;

    // Get a list of all network interfaces
    if (getifaddrs(&ifaddr) == -1) {
        perror("getifaddrs failed");
        return -1;
    }

    // Iterate through the list of interfaces
    for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next) {
        if (ifa->ifa_addr == NULL) continue;

        family = ifa->ifa_addr->sa_family;

        // Check for IPv4 addresses only
        if (family == AF_INET) {
            // Ignore loopback interface (127.0.0.1)
            if (strcmp(ifa->ifa_name, "lo") == 0) continue;

            // Convert the address to a string and store it in ip_buffer
            if (inet_ntop(family, &((struct sockaddr_in *)ifa->ifa_addr)->sin_addr,
                          ip_buffer, buffer_size) == NULL) {
                perror("inet_ntop failed");
                freeifaddrs(ifaddr);
                return -1;
            }

            // Mark as found and break out of the loop
            found = 1;
            break;
        }
    }

    freeifaddrs(ifaddr);

    if (!found) {
        fprintf(stderr, "No suitable IPv4 address found\n");
        return -1;
    }

    return 0;
}

// Get number of connected clients.
void get_clients_count(int client_sock){
    char buf[2];
    sprintf(buf, "%d", connectedClients);
    send(client_sock, buf, 2, 0);
    return;
}
