#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <signal.h>
#include <stdlib.h>

# define MAXLINE 4096

static int count;

static void recvfrom_int(int signo) {
    printf("\n received %d datagrams, stop by signal: %d. \n", count, signo);
    exit(0);
}

int main(int argc, char **argv) {
    int sock_fd = socket(AF_INET, SOCK_DGRAM, 0);

    struct sockaddr_in server_addr;
    bzero(&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(12345);

    bind(sock_fd, (struct sockaddr *) &server_addr, sizeof(server_addr));

    socklen_t client_len;
    char message[MAXLINE];
    count = 0;

    // 键盘 ctrl + c 命令发出的信号
    signal(SIGINT, recvfrom_int);
    // ide 发送的终止信号是 SIGTERM，这里也捕获一下
    signal(SIGTERM, recvfrom_int);

    struct sockaddr_in client_addr;
    client_len = sizeof(client_addr);

    for (;;) {
        // 服务端可以接收来自多个 client 的消息，因此这里在接收的时候需要指定一下对端地址存放指针
        // 通过读取这个指针指向的内容，我们就能知道是哪个 client 发送过来的 udp 包了（udp 无连接，因此不知道连接上下文，当然也不知道是谁发送过来的消息）。
        int n = recvfrom(sock_fd, message, MAXLINE, 0, (struct sockaddr *) &client_addr, &client_len);
        message[n] = '\0';
        printf("received %d bytes: %s. \n", n, message);

        char send_line[MAXLINE];
        sprintf(send_line, "Hi, %s", message);

        // 将 echo 消息按照接收到的地址发送回去，也就是消息从哪里来，就返回到哪里去.
        sendto(sock_fd, send_line, strlen(send_line), 0, (struct sockaddr *) &client_addr, client_len);
        // 接收数据个数统计
        count++;
    }
}