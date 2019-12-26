#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <error.h>
#include <errno.h>
#include <signal.h>
#include <unistd.h>

#define PORT 12345
#define MAXLINE 4096

static int count;

static void sig_int(int signo) {
    printf("\n received %d datagram, end by signo: %d.\n", count, signo);
    exit(0);
}

int main(int argc, char **argv) {
    int listen_fd = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in server_addr;
    bzero(&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(PORT);

    int bind_rt = bind(listen_fd, (struct sockaddr *) &server_addr, sizeof(server_addr));
    if (bind_rt < 0) {
        error(1, errno, "bind failed.");
    }

    int listen_rt = listen(listen_fd, 10);
    if (listen_fd < 0) {
        error(1, errno, "listen failed.");
    }

    signal(SIGINT, sig_int);
    // 当 client close 掉 socket 之后，第一次我们往 socket 中写数据会收到 RST 分组
    // 第二次再次写入的时候会触发 signal SIGPIPE，所以这里我们处理一下这个signal，让服务端正常退出并打印统计信息
    signal(SIGPIPE, sig_int);

    int conn_fd;
    struct sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);

    if ((conn_fd = accept(listen_fd, (struct sockaddr *) &client_addr, &client_len)) < 0) {
        error(1, errno, "bind failed.");
    }

    char message[MAXLINE];
    count = 0;

    for (;;) {
        int n = read(conn_fd, message, MAXLINE);
        if (n < 0) {
            error(1, errno, "read failed.");
        } else if (n == 0) {
            error(1, 0, "client close.");
        }

        message[n] = 0;
        printf("receive %d bytes: %s\n", n, message);
        count++;

        char send_line[MAXLINE];
        sprintf(send_line, "Hi, %s", message);
        sleep(5);

        int write_nc = send(conn_fd, send_line, strlen(send_line), 0);
        printf("send bytes: %d. \n", write_nc);
        if (write_nc < 0) {
            error(1, errno, "write failed.");
        }
    }
}