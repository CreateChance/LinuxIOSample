
#include <error.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>

#define MAXLINE 4096

int main(int argc, char **argv) {
    if (argc != 2) {
        error(1, 0, "usage: udp_client <ip address>");
    }

    int sock_fd = socket(AF_INET, SOCK_DGRAM, 0);

    struct sockaddr_in server_addr;
    bzero(&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(12345);
    inet_pton(AF_INET, argv[1], &server_addr.sin_addr);

    socklen_t server_len = sizeof(server_addr);

    struct sockaddr *replay_addr = malloc(server_len);

    char send_line[MAXLINE], recv_line[MAXLINE + 1];
    socklen_t len;
    int n;

    while (fgets(send_line, MAXLINE, stdin) != NULL) {
        int i = strlen(send_line);
        if (send_line[i - 1] == '\n') {
            send_line[i - 1] = 0;
        }

        printf("now sending: %s. \n", send_line);

        // udp 无连接，因此不需要建立连接
        // 但是发送的时候，需要指定对端的地址和端口信息，否则 udp 数据包不知道往哪里发送（因为没有连接作为上下文）
        size_t rt = sendto(sock_fd, send_line, strlen(send_line), 0, (struct sockaddr *) &server_addr, server_len);
        if (rt < 0) {
            error(1, errno, "send failed.");
        }
        printf("send bytes: %zu \n", rt);

        len = 0;
        // 接收 udp 数据的时候也需要指定一下地址接收的指针
        // 接收成功之后这个指针指向发送端地址的内存块，通过读取这个内存块就可以知道是谁发送过来的这个 udp 消息了
        n = recvfrom(sock_fd, recv_line, MAXLINE, 0, replay_addr, &len);
        if (n < 0) {
            error(1, errno, "receive failed.");
        }
        recv_line[n] = '\0';
        fputs(recv_line, stdout);
        fputs("\n", stdout);
    }

    return 0;
}