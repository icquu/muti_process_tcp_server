#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <strings.h>

/*
Invest 5 yuan, double the investment each time, 
default 1 million consecutive guesses, 
test the maximum investment funds, 
you can customize the total number of guesses
*/

int even_measure(int times);

int main(int argc, char **argv){
  
  int b = 0;
  int c = 0, e = 5;
  long total_spend=0;
  int ret = 0;
  
  struct sockaddr_in serveraddr;
  bzero(&serveraddr, sizeof(serveraddr));
  serveraddr.sin_family = AF_INET;
  serveraddr.sin_port = htons(6666);
  serveraddr.sin_addr.s_addr = inet_addr("192.168.15.122");
  
  int sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if(sockfd<0){
    perror("sockfd");
    exit(1);
  }
  int flag = 1;
  setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &flag, sizeof(flag));
  
  ret = bind(sockfd, (const struct sockaddr *)&serveraddr, sizeof(serveraddr));
  if(ret<0){
    perror("sockfd");
    exit(1);
  }
  
  ret = listen(sockfd, 128);
  if(ret<0){
    perror("listen");
    exit(1);
  }
  
 
  while(1){
    int cfd = accept(sockfd, NULL, NULL);
    if(cfd<0){
      perror("accept");
      exit(1);
    }
    dprintf(cfd, "Maximum number of consecutive times:%d\n", cfd);
    close(cfd);
//       event.data.fd=cfd;
//       event.events= EPOLLIN;
//       epoll_ctl(epfd, EPOLL_CTL_ADD, cfd, &event);
  }
  return 0;  
}

