#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <pthread.h>

#define PORT 12345

/*
 * This will handle connection for each client
 * */
void *connection_handler(void *socket_desc) {
    //Get the socket descriptor
    int sock = *(int *) socket_desc;
    int read_size;
    char buffer[1024] = {0};
    while (1) {
        // read client data.
        read_size = read(sock, buffer, 1024);
        printf("Read: %s. \n", buffer);
        if (strncmp("server_quit", buffer, sizeof(buffer)) == 0) {
            break;
        }
        // write back data.
        send(sock, buffer, strlen(buffer), 0);
    }

    return 0;
}

int main(int argc, char const *argv[]) {
    int server_fd, client_socket;
    struct sockaddr_in server, client;
    int opt = 1;
    int addrlen = sizeof(server);

    // create socket fd
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        perror("set socket option failed.");
        exit(EXIT_FAILURE);
    }

    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(PORT);

    // forcefully attaching socket to port
    if (bind(server_fd, (struct sockaddr *) &server, sizeof(server)) < 0) {
        perror("bind failed.");
        exit(EXIT_FAILURE);
    }

    // listen
    if (listen(server_fd, 3) < 0) {
        perror("listen failed.");
        exit(EXIT_FAILURE);
    }

    while ((client_socket = accept(server_fd, (struct sockaddr *) &client, (socklen_t *) &addrlen))) {
        puts("Connection accepted.");
        pthread_t handle_thread;
        int *new_socket = malloc(1);
        *new_socket = client_socket;
        if (pthread_create((pthread_t *) &handle_thread, NULL, connection_handler, (void *) new_socket) < 0) {
            perror("Create handle thread failed.");
            exit(EXIT_FAILURE);
        }

        puts("Handle client connection done!");
    }

    puts("Server dead!");

    return 0;
}