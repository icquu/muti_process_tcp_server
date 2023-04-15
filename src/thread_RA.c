#include <pthread.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <math.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>
#include <strings.h>
#include <sys/wait.h>

/*
Invest 5 yuan, double the investment each time, 
default 1 million consecutive guesses, 
test the maximum investment funds, 
you can customize the total number of guesses
*/

int even_measure(int times);

void* pthread_fun(void* arg){
  int cfd = (int)arg;
  long total_spend=0;
  int c = 0, e = 5;
//  c=even_measure(100);
  c=100;
  for(int j=0;j<c;j++){
    e*=2;
    total_spend+=e;
  }
  dprintf(cfd, "Maximum number of consecutive times:%d\n", c);
  dprintf(cfd, "Maximum single investment:%d\n", e);
  dprintf(cfd, "Total investment:%d\n", total_spend);
  shutdown(cfd, SHUT_WR);
  usleep(20000);
  shutdown(cfd, SHUT_RD);
  usleep(20000);
  close(cfd);
  
  return (void*)0;
}

int main(int argc, char **argv){

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
  
  ret = listen(sockfd, 12800);
  if(ret<0){
    perror("listen");
    exit(1);
  }
  
  pthread_t pt;
  pthread_attr_t attr;
  pthread_attr_init(&attr);
  ret = pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
  if (ret != 0){
    perror("pthread_attr_setdetachstate");
    exit(1);
  }
  
  while(1){
    int cfd = accept(sockfd, NULL, NULL);
    if(cfd<0){
      perror("accept");
      exit(1);
    }
    printf("new cfd: %d\n", cfd);
    ret = pthread_create(&pt, &attr, pthread_fun, (void *)cfd);
    if (ret != 0){
      perror("pthread_create");
      exit(1);
    }
  }
  pthread_attr_destroy(&attr);
  
  return 0;
}

int even_measure(int times){
  int max_con_times=0;
  int a=0, b=0;
  srand((unsigned)pthread_self());
  for(int i=0;i<times;i++){
    a = rand();
    if(a%2==0){
      b++;
    }else{
      if(b>max_con_times){
        max_con_times=b;
      }
      b=0;
    }
  }
  return max_con_times;
}
