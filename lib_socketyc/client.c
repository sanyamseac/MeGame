#include "netimports.h"

#define MAX_LEN 1024
void send_command(int sockfd, const char *command, char* results) {
    // char buffer[BUFFER_SIZE] = {0};
    memset(results, 0, MAX_LEN);
    send(sockfd, command, strlen(command), 0);
    recv(sockfd, results, MAX_LEN, 0);
    // printf("Server response: %s\n", buffer);
}

int connect_to_server(char* srvr_addr, int port){
    int sockfd;
    struct sockaddr_in server_addr;

    // Create socket
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Socket creation error");
        exit(EXIT_FAILURE);
    }

    // Configure server address
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = inet_addr(srvr_addr); 

    // Connect to server
    if (connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Connection failed");
        close(sockfd);
        return -1;
    }
    return sockfd;
}

int init_player(int sockfd, char* nickname){
    char commandBuff[MAX_LEN];
    sprintf(commandBuff, "CREATE %s", nickname);
    int clientID = 0;
    char buff[MAX_LEN];
    send_command(sockfd, commandBuff, buff);
    sscanf(buff, "%d", &clientID);
    close(sockfd);
    return clientID;
}

void read_data(int sockfd, int clientID, char* buffer){
    char commandBuff[MAX_LEN];
    sprintf(commandBuff, "READ %d %s", clientID, " ");
    send_command(sockfd, commandBuff, buffer);
    close(sockfd);
}

int update_data(int sockfd, int clientID, char* data){
    char commandBuff[MAX_LEN];
    sprintf(commandBuff, "UPDATE %d %s", clientID, data);
    int result = 0;
    char buff[MAX_LEN];
    send_command(sockfd, commandBuff, buff);
    sscanf(buff, "%d", &result);
    close(sockfd);
    return result;
}
int disconnect_player(int sockfd, int clientID){
    char commandBuff[MAX_LEN];
    sprintf(commandBuff, "DELETE %d %s", clientID, " ");
    int result = 0;
    char buff[MAX_LEN];
    send_command(sockfd, commandBuff, buff);
    sscanf(buff, "%d", &result);
    close(sockfd);
    return result;
}
int get_connected_count(int sockfd){
    char commandBuff[MAX_LEN];
    sprintf(commandBuff, "COUNT %d %s", 0, " ");
    int count = 0;
    char buff[MAX_LEN];
    send_command(sockfd, commandBuff, buff);
    sscanf(buff, "%d", &count);
    close(sockfd);
    return count;
}


// int main() {
//     char* addr = "127.0.0.1";
//     int port = 8000;
    
//     int clientID = 0;

//     clientID = init_player(connect_to_server(addr, port), "Raylib");
//     printf("Connected people: %d\n", get_connected_count(connect_to_server(addr, port)));
//     char buff[MAX_LEN];
//     read_data(connect_to_server(addr, port), clientID, buff);
//     printf("Data: %s\n", buff);
//     printf("Update?: %d\n", update_data(connect_to_server(addr, port), clientID, "Crying"));
//     read_data(connect_to_server(addr, port), clientID, buff);
//     printf("Data: %s\n", buff);
//     printf("Disconnect: %d\n", disconnect_player(connect_to_server(addr, port), clientID));
//     printf("Connected people: %d\n", get_connected_count(connect_to_server(addr, port)));

//     clientID = init_player(connect_to_server(addr, port), "Raysan");
//     printf("Connected people: %d\n", get_connected_count(connect_to_server(addr, port)));
//     // char buff[MAX_LEN];
//     read_data(connect_to_server(addr, port), clientID, buff);
//     printf("Data: %s\n", buff);
//     printf("Update?: %d\n", update_data(connect_to_server(addr, port), clientID, "dying"));
//     read_data(connect_to_server(addr, port), clientID, buff);
//     printf("Data: %s\n", buff);
//     printf("Disconnect: %d\n", disconnect_player(connect_to_server(addr, port), clientID));
//     printf("Connected people: %d\n", get_connected_count(connect_to_server(addr, port)));
    
    
//     return 0;
// }
