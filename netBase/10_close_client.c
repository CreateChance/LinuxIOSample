
#include <error.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

#define MAXLINE 4096
#define PORT 12345

int main(int argc, char **argv) {
    if (argc != 2) {
        error(1, 0, "usage: client <ip address>");
    }

    // use tcp protocol
    int sock_fd = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in server_addr;
    bzero(&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    inet_pton(AF_INET, argv[1], &server_addr.sin_addr);

    socklen_t server_len = sizeof(server_addr);
    int conn_ret = connect(sock_fd, (struct sockaddr *) &server_addr, server_len);
    if (conn_ret < 0) {
        error(1, errno, "connect failed.");
    }

    char send_line[MAXLINE], recv_line[MAXLINE + 1];
    int n;

    fd_set read_mask;
    fd_set all_reads;

    FD_ZERO(&all_reads);
    FD_SET(0, &all_reads);
    FD_SET(sock_fd, &all_reads);

    for (;;) {
        read_mask = all_reads;

        int rc = select(sock_fd + 1, &read_mask, NULL, NULL, NULL);
        if (rc < 0) {
            error(1, errno, "select failed.");
        }

        if (FD_ISSET(sock_fd, &read_mask)) {
            // socket 上有数据了，需要读一下
            n = read(sock_fd, recv_line, MAXLINE);
            if (n < 0) {
                error(1, errno, "read error.");
            } else if (n == 0) {
                // server close this socket, we can't read any more.
                error(1, 0, "server terminated.");
            }

            recv_line[n] = 0;
            fputs(recv_line, stdout);
            fputs("\n", stdout);
        }
        if (FD_ISSET(0, &read_mask)) {
            if (fgets(send_line, MAXLINE, stdin) != NULL) {
                if (strncmp(send_line, "shutdown", 8) == 0) {
                    FD_CLR(0, &all_reads);
                    if (shutdown(sock_fd, SHUT_WR)) {
                        error(1, errno, "shutdown failed.");
                    }
                } else if (strncmp(send_line, "close", 5) == 0) {
                    FD_CLR(0, &all_reads);
                    if (close(sock_fd)) {
                        error(1, errno, "close failed.");
                    }
                    sleep(6);
                    exit(0);
                } else {
                    int i = strlen(send_line);
                    if (send_line[i - 1] == '\n') {
                        send_line[i - 1] = 0;
                    }

                    printf("now sending %s \n", send_line);
                    size_t rt = write(sock_fd, send_line, strlen(send_line));
                    if (rt < 0) {
                        error(1, errno, "write failed.");
                    }
                    printf("send bytes: %zu \n", rt);
                }
            }
        }
    }
}