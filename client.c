#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/wait.h>

#define DEST_PORT 1500
#define DEST_IP "127.0.0.1"
#define MAX_DATA 10000


int main() {
    int socket_fd, adverse_fd;

    struct sockaddr_in dest_addr;

    char buf[MAX_DATA];

    socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_fd == -1)
    {
        printf("Socket failed: %d!\n", errno);
    }

    dest_addr.sin_family = AF_INET;
    dest_addr.sin_port = htons(DEST_PORT);
    dest_addr.sin_addr.s_addr = inet_addr(DEST_IP);
    bzero(&(dest_addr.sin_zero), 8);
    
    if (connect(socket_fd, (struct sockaddr*)&dest_addr, sizeof(struct sockaddr)) == 1) {
        printf("Connect failed: %d!\n", errno);
    }
    else
    {
        printf("Connect success!\n");
        recv(socket_fd, buf, MAX_DATA, 0);
        printf("Received: %s\n", buf);
    }

    close(socket_fd);

    return 0;
}