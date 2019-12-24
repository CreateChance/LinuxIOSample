#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>

#define PORT 12345

int main(int argc, char const *argv[]) {
    int sock = 0;
    struct sockaddr_in serv_addr;
    char send_buffer[1024] = {0};
    char read_buffer[1024] = {0};
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("\n Socket creation error \n");
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    // Convert IPv4 and IPv6 addresses from text to binary form
    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
        printf("\nInvalid address/ Address not supported \n");
        return -1;
    }

    if (connect(sock, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        printf("\nConnection Failed \n");
        return -1;
    }

    while (1) {
        fgets(send_buffer, 1024, stdin);
        if (strncmp("quit", send_buffer, 4) == 0) {
            break;
        }
        send(sock, send_buffer, strlen(send_buffer) - 1, 0);
        // then read echo message.
        int read_bytes = read(sock, read_buffer, 1024);
        read_buffer[read_bytes] = '\0';
        printf("Echo: %s, read bytes: %d. \n", read_buffer, read_bytes);
    }

    return 0;
}