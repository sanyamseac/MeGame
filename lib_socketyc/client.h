#ifndef LIBSOCKETCCLIENT
#define LIBSOCKETCCLIENT


int connect_to_server(char* srvr_addr, int port);
int init_player(int sockfd, char* nickname);
void read_data(int sockfd, int clientID, char* buffer);
int update_data(int sockfd, int clientID, char* data);
int disconnect_player(int sockfd, int clientID);
int get_connected_count(int sockfd);

#endif