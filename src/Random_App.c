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

void sig_handler(int sig){
  waitpid(0, NULL, 0);
}

int main(int argc, char **argv){
  signal(SIGCHLD, sig_handler);
  
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
  
begin_ball:
  int cfd = accept(sockfd, NULL, NULL);
  if(cfd<0){
    perror("accept");
    exit(1);
  }
  
  pid_t pt=fork();
  if(pt<0){
    perror("accept");
    exit(1);
  }
  
  if(pt==0){
    close(sockfd);
    if(argc==2){
      c=even_measure(atoi(argv[1]));
    }else{
      c=even_measure(1000000);
    }
    
    for(int j=0;j<c;j++){
      e*=2;
      total_spend+=e;
    }
    dup2(cfd, STDOUT_FILENO);
    printf("Maximum number of consecutive times:%d\n", c);
    printf("Maximum single investment:%d\n", e);
    printf("Total investment:%d\n", total_spend);
    close(cfd);
    close(STDOUT_FILENO);
    exit(0);
  }
  if(pt>0){
    close(cfd);
    goto begin_ball;
  }
  return 0;
}

int even_measure(int times){
  int max_con_times=0;
  int a=0, b=0;
  srand((unsigned)time(NULL));
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
