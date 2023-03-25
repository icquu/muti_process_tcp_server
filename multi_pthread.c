#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <arpa/inet.h>

//#define debug

typedef struct _person{
  int cfd;
  char name[16];
}person;

person plist[100]; //这里使用链表，理论上可以支持无限多个用户同时聊天。使用红黑二叉树可以快速查找某个人是否存在。如果把个人信息和聊天记录存库，可以实现登录验证和聊天记录重现。可以借助数据库实现查询，但实现客户端本地查询可以大大降低服务端压力。

int count=0; //在线人数
    
void* p_fun(void *arg){
  char buf[BUFSIZ];
  memset(buf,'\0', BUFSIZ);
  int cfd = (int)arg;
  
  count++; //在线人数加1
  plist[cfd%100].cfd = cfd; //获取当前socket会话
  write(cfd,"input your nickname which is show to others when you talk to others: ",70);
  read(cfd,plist[cfd%100].name,16);
#ifdef debug
  printf("%d %s\n", cfd, plist[cfd%100].name); //打印当前会话id和人物昵称
#endif
  char echo_buf[32]={'\0'};
  sprintf(echo_buf, "there're %d online\n", count); //讲当前在线人数写入echo_buf
  write(cfd, echo_buf, 32);
  for (int i=0;i<100;i++){
    if (plist[i].cfd>0){
      if(plist[i].cfd != cfd){
        write(plist[i].cfd, "welcome ", 9);
        write(plist[i].cfd, plist[cfd%100].name, 16);
        write(plist[i].cfd, echo_buf, 32);
      }
    }
  }
  
  while (read(cfd, buf, BUFSIZ)){
#ifdef debug    
    for(int i=0;plist[cfd%100].name[i]!='\n' && plist[cfd%100].name[i]!='\0';i++){
      printf("%c",plist[cfd%100].name[i]);
    }
    printf(" say's: %s", buf);
#endif
    char true_name[41]={'\0'};    
    for (int i=0;i<100;i++){
      if (plist[i].cfd>0 && plist[i].cfd != cfd){
        sprintf(true_name,"----------------\n");
        //write(plist[i].cfd,plist[cfd%100].name,16);
        for(int j=0;plist[cfd%100].name[j]!='\0' && plist[cfd%100].name[j]!='\n';j++){
          //write(plist[i].cfd,&plist[cfd%100].name[j],1);
          sprintf(true_name+17+j,"%c", plist[cfd%100].name[j]);
        }
        strcat(true_name, " say's: ");
        write(plist[i].cfd, true_name, 41);
        write(plist[i].cfd,buf,BUFSIZ);
        write(plist[i].cfd,"----------------\n",17);
      }
    }
    memset(buf, '\0', BUFSIZ);
  }
  close(cfd); //关闭socket描述符。
  //用户断开连接，通知在线用户谁退出了，打印当前在线人数和在线人昵称列表。
  count--;
  memset(echo_buf, '\0', 32);
  sprintf(echo_buf, "there're %d online\n", count);
  for (int i=0;i<100;i++){
      if (plist[i].cfd>0 && plist[i].cfd != cfd){
        write(plist[i].cfd,"----------------\n",17);
        //write(plist[i].cfd,plist[cfd%100].name,16);
        for(int j=0;plist[cfd%100].name[j]!='\0' && plist[cfd%100].name[j]!='\n';j++){
          write(plist[i].cfd,&plist[cfd%100].name[j],1);
        }
        write(plist[i].cfd, " exit.\n", 7);
        write(plist[i].cfd,"----------------\n",17);
        write(plist[i].cfd, echo_buf, 32);
        write(plist[i].cfd,"online person:\n",15);
        for (int k=0; k<100; k++){
          if (plist[k].name[0]!='\0' && strcmp(plist[cfd%100].name, plist[k].name))
            write(plist[i].cfd, plist[k].name, 16);
        }
        write(plist[i].cfd,"----------------\n",17);
      }
    }
  plist[cfd%100].cfd = 0;
  memset(plist[cfd%100].name,'\0',16);
  return (void*)0;
}

int main(int argc, char **argv){

  for (int i=0;i<100;i++){
    plist[i].cfd = 0;
    memset(plist[i].name,'\0',16);
  }

  struct sockaddr_in serveraddr, clientaddr;
  int socklength = sizeof(serveraddr), flag=1;
  bzero(&serveraddr,socklength);
  bzero(&clientaddr,socklength);
  
  serveraddr.sin_family = AF_INET;
  serveraddr.sin_port = htons(6666);
  serveraddr.sin_addr.s_addr = htonl(0);
  
  int lfd = socket(AF_INET, SOCK_STREAM, 0);
  if (lfd == -1){
    perror("socket");
    exit(1);
  }
  setsockopt(lfd, SOL_SOCKET, SO_REUSEADDR, &flag, sizeof(flag));
  int ret = bind(lfd, (const struct sockaddr*)&serveraddr, socklength);
  if (ret == -1){
    perror("bind");
    exit(1);
  }
  ret = listen(lfd, 128);
  if (ret == -1){
    perror("listen");
    exit(1);
  }
  
  pthread_t pt;
  pthread_attr_t attr;
  pthread_attr_init(&attr);
  ret = pthread_attr_setdetachstate(&attr,PTHREAD_CREATE_DETACHED);
  if (ret != 0){
    perror("pthread_attr_setdetachstate");
    exit(1);
  }
  while (1){
    int cfd = accept(lfd,NULL,NULL);
    pthread_create(&pt,&attr,p_fun,(void*)cfd);
    //pthread_join(pt,NULL);
  }

  return 0;
}
