
#include <error.h>
#include <unistd.h>
#include <sys/socket.h>
#include <linux/un.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>

int main(int argc, char **argv) {
    if (argc != 2) {
        error(1, 0, "usage: server <local_path>");
    }

    int listen_fd, conn_fd;
    socklen_t client_len;
    struct sockaddr_un client_addr, server_addr;

    listen_fd = socket(AF_LOCAL, SOCK_STREAM, 0);
    if (listen_fd < 0) {
        error(1, errno, "socket create failed.");
    }

    char *local_path = argv[1];
    unlink(local_path);

    bzero(&server_addr, sizeof(server_addr));
    server_addr.sun_family = AF_LOCAL;
    strcpy(server_addr.sun_path, local_path);

    if (bind(listen_fd, (struct sockaddr *) &server_addr, sizeof(server_addr)) < 0) {
        error(1, errno, "bind failed.");
    }

    if (listen(listen_fd, 10) < 0) {
        error(1, errno, "listen failed.");
    }

    client_len = sizeof(client_addr);

    if ((conn_fd = accept(listen_fd, (struct sockaddr *) &client_addr, &client_len)) < 0) {
        if (errno == EINTR) {
            error(1, errno, "accept failed.");
        } else {
            error(1, errno, "accept failed.");
        }
    }

    char buf[1024];

    while (1) {
        bzero(buf, sizeof(buf));
        if (read(conn_fd, buf, 1024) == 0) {
            printf("client quit.");
            break;
        }
        printf("Receive: %s", buf);

        char send_line[1024];
        sprintf(send_line, "Hi, %s", buf);

        int nbytes = sizeof(send_line);

        if (write(conn_fd, send_line, nbytes) != nbytes) {
            error(1, errno, "write error.");
        }
    }

    close(listen_fd);
    close(conn_fd);

    return 0;
}