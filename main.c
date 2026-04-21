#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define BUFFER_SIZE 1024

int main(void) {
    int server, client;
    struct sockaddr_in address;
    memset(&address, 0, sizeof(address));

    int addrlen = sizeof(address);
    char buffer[BUFFER_SIZE] = {0};

    server = socket(AF_INET, SOCK_STREAM, 0);
    if (server == -1) {
        printf("file descriptor error\n");
        exit(0);
    } else {
        printf("socket created!\n");
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server, (struct sockaddr*)&address, sizeof(address)) != 0) {
        printf("bind not success!\n");
        exit(0);
    } else
        printf("binding successful\n");
    listen(server, 3);

    printf("waiting for the connection...\n");

    client = accept(server, (struct sockaddr*)&address, (socklen_t*)&addrlen);

    int byte_read = read(client, buffer, 1024);
    printf("total bytes read: %d\n", byte_read);
    printf("Client says : %s\n", buffer);

    char *msg = "hello from server!\n";
    send(client, msg, strlen(msg), 0);

    close(client);
    close(server);

    return 0;
}

/*
  Function	Description
    - htons()	host to network short
    - htonl()	host to network long
    - ntohs()	network to host short
    - ntohl()	network to host long
 */
