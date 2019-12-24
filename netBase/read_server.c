/**
 * 重点讲述了通过 send 和 read 来收发数据包，
 * 你需要牢记以下两点：对于 send 来说，返回成功仅仅表示数据写到发送缓冲区成功，并不表示对端已经成功收到。
 * 对于 read 来说，需要循环读取数据，并且需要考虑 EOF 等异常条件。
 */

#include <unistd.h>
#include <netinet/in.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>

size_t read_n(int fd, void *buffer, size_t size) {
    char *buffer_pointer = buffer;
    int length = size;

    while (length > 0) {
        int result = read(fd, buffer_pointer, length);

        if (result < 0) {
            if (errno == EINTR) {
                continue;
            } else {
                return -1;
            }
        } else if (result == 0) {
            break;
        }

        length -= result;
        buffer_pointer += result;
    }
    return (size - length);
}

void read_data(int sock_fd) {
    ssize_t n;
    char buf[1024];

    int time = 0;
    for (;;) {
        fprintf(stdout, "block in read. \n");
        if ((n = read_n(sock_fd, buf, 1024)) == 0) {
            return;
        }

        time++;
        fprintf(stdout, "1K read for %d time. \n", time);
        // 休眠一段时间模拟业务处理
        usleep(1000);
    }
}

int main(int argc, char **argv) {
    int listen_fd, conn_fd;
    socklen_t client_len;
    struct sockaddr_in client_addr;
    struct sockaddr_in server_addr;

    listen_fd = socket(AF_INET, SOCK_STREAM, 0);

    bzero(&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(12345);

    bind(listen_fd, (struct sockaddr *) &server_addr, sizeof(server_addr));

    listen(listen_fd, 1024);

    for (;;) {
        client_len = sizeof(client_addr);
        conn_fd = accept(listen_fd, (struct sockaddr *) &client_addr, &client_len);
        read_data(conn_fd);
        close(conn_fd);
    }
}