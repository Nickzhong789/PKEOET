#include <pbc/pbc.h>
#include <pbc/pbc_test.h>
#include <string.h>
#include <gmp.h>
#include <stdio.h>
#include <stdlib.h>

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


int i;

pairing_t pairing;

void init(element_t temp_t[]) {
  for (i = 0; i < 4; i++)
  {
    element_init_G1(temp_t[i], pairing);
    element_random(temp_t[i]);

    element_printf("v%d is %B\n", i, temp_t[i]);
  }
  
}


int main(int argc, char **argv) {
    pbc_demo_pairing_init(pairing, argc, argv);

    element_t v[4];
    init(v);

    int socket_fd, adverse_fd;

    struct sockaddr_in dest_addr;

    char buf[MAX_DATA];

    int b = 666;
    int c;

    socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_fd == -1)
    {
        printf("Socket failed: %d!\n", errno);
        exit(-1);
    }

    dest_addr.sin_family = AF_INET;
    dest_addr.sin_port = htons(DEST_PORT);
    dest_addr.sin_addr.s_addr = inet_addr(DEST_IP);
    bzero(&(dest_addr.sin_zero), 8);
    
    if (connect(socket_fd, (struct sockaddr*)&dest_addr, sizeof(struct sockaddr)) == 1) {
        printf("Connect failed: %d!\n", errno);
        exit(-1);
    }
    else
    {
        printf("Connect success!\n");
        for (i = 0; i < 4; i++)
        {
            send(adverse_fd, (void*)&b, sizeof(b), 0);
            printf("Send %d\n", i);
        }
        
        //send(adverse_fd, (void*)&b, sizeof(b), 0);

        //recv(socket_fd, (void*)&c, sizeof(c), 0);
        //element_printf("Received: %B\n", e);
        //printf("Received: %d\n", c);
    }

    close(socket_fd);

    return 0;
}