#include <pbc/pbc.h>
#include <pbc/pbc_test.h>
#include <string.h>
#include <gmp.h>
#include <stdio.h>
#include <stdlib.h>

#include<stdio.h>
#include<stdlib.h>
#include<errno.h>
#include<string.h>
#include<sys/types.h>
#include<netinet/in.h>
#include<sys/socket.h>
#include<sys/wait.h>
 
#define PORT 1500//端口号 
#define BACKLOG 5/*最大监听数*/


element_t v[4];
pairing_t pairing;

int i;

int main(int argc, char **argv) {
  pbc_demo_pairing_init(pairing, argc, argv);

  //init(v);

  int socket_fd, adverse_fd;

  struct sockaddr_in my_addr;
  struct sockaddr_in adverse_addr;

  unsigned int sin_size;

  int a;
  printf("Server start..................\n");

  socket_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (socket_fd == -1)
  {
    printf("Socket failed: %d\n", errno);
    return -1;
  }

  my_addr.sin_family = AF_INET;
  my_addr.sin_port = htons(PORT);
  //my_addr.sin_addr.s_addr = htonl(INADDR_ANY);
  my_addr.sin_addr.s_addr = htonl(INADDR_ANY);
  bzero(&(my_addr.sin_zero), 8);

  if (bind(socket_fd, (struct sockaddr*)&my_addr, sizeof(struct sockaddr)) < 0)
  {
    printf("Bind error!\n");
    return -1;
  }
  
  listen(socket_fd, BACKLOG);

  while (1)
  {
    sin_size = sizeof(struct sockaddr_in);
    adverse_fd = accept(socket_fd, (struct sockaddr*)&adverse_addr, &sin_size);

    if (adverse_fd == -1)
    {
      printf("Receive failed!\n");
    }
    else
    {
      printf("Receive success!\n");
      recv(socket_fd, (void*)&a, sizeof(a), 0);
      //send(adverse_fd, (void*)&a, sizeof(a), 0);
      printf("Receive: %d\n", a);
    }
  }
  
  return 0;
}
