#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <stdio.h>
#include <error.h>
#include <errno.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define MESSAGE_SIZE 10240000

void send_data(int sock_fd) {
    char *query = malloc(MESSAGE_SIZE + 1);
    for (int i = 0; i < MESSAGE_SIZE; i++) {
        query[i] = 'a';
    }
    query[MESSAGE_SIZE] = '\0';

    const char *cp = query;
    size_t remaining = strlen(query);

    // 循环发送指定大小的数据，一次可能发送不完，没关系，循环发送就好
    while (remaining) {
        int n_written = send(sock_fd, cp, remaining, 0);
        fprintf(stdout, "send into buffer %d. \n", n_written);
        // 小于 0 表示发送失败了
        if (n_written <= 0) {
            error(1, errno, "send failed.");
            return;
        }
        remaining -= n_written;
        cp += n_written;
    }
}

int main(int argc, char **argv) {
    int sock_fd;
    struct sockaddr_in server_addr;

    if (argc != 2) {
        error(1, 0, "usage: read_client <ip address>");
    }

    sock_fd = socket(AF_INET, SOCK_STREAM, 0);

    bzero(&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(12345);
    inet_pton(AF_INET, argv[1], &server_addr.sin_addr);

    int conn_ret = connect(sock_fd, (struct sockaddr *) &server_addr, sizeof(server_addr));
    if (conn_ret < 0) {
        error(1, errno, "connect failed.");
    }

    send_data(sock_fd);

    return 0;
}