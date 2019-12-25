
#include <error.h>
#include <linux/un.h>
#include <sys/socket.h>
#include <bits/socket.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>

int main(int argc, char **argv) {
    if (argc != 2) {
        error(1, 0, "usage: client <local_path>");
    }

    int sock_fd;
    struct sockaddr_un server_addr;

    sock_fd = socket(AF_LOCAL, SOCK_STREAM, 0);
    if (sock_fd < 0) {
        error(1, errno, "create socket failed.");
    }

    bzero(&server_addr, sizeof(server_addr));
    server_addr.sun_family = AF_LOCAL;
    strcpy(server_addr.sun_path, argv[1]);

    if (connect(sock_fd, (struct sockaddr *) &server_addr, sizeof(server_addr)) < 0) {
        error(1, errno, "connect failed.");
    }

    char send_line[1024];
    bzero(send_line, sizeof(send_line));
    char recv_line[1024];

    while (fgets(send_line, 1024, stdin) != NULL) {
        int nbytes = sizeof(send_line);
        if (write(sock_fd, send_line, nbytes) != nbytes) {
            error(1, errno, "write error.");
        }

        if (read(sock_fd, recv_line, 1024) == 0) {
            error(1, errno, "server quit prematurely");
        }

        fputs(recv_line, stdout);
    }

    return 0;
}