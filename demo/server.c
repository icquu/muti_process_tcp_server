#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define SOCEKT_FAILL (-1)

int main()
{
    int res;
    char buff[BUFSIZ];
    char sendBUFF[BUFSIZ];
    int on = 1;
    time_t time_now;

    struct sockaddr_in servAddr;
    memset(&servAddr, 0, sizeof(servAddr));
    servAddr.sin_family = AF_INET;
    servAddr.sin_addr.s_addr = INADDR_ANY;
    servAddr.sin_port = htons(8899);

    printf("*** Server Start! ***\n");

    int servSocket = socket(AF_INET, SOCK_STREAM, 0); //创建soket
    setsockopt(servSocket,SOL_SOCKET,SO_REUSEADDR, &on, sizeof(on)); //跳过socket的timewait时间

    if (servSocket == SOCEKT_FAILL){
        printf("Create Soket Fail!\n");
        return 0;
    }

    while(bind(servSocket, (struct sockaddr*)&servAddr, sizeof(servAddr)) == SOCEKT_FAILL); //绑定socket
    printf("Bind Success!\n");

    while(listen(servSocket, 10) == SOCEKT_FAILL); //进入监听状态
    printf("Listening....\n");
    printf("Ready for Accept,Waitting...\n");


    int clntSock  = accept(servSocket, (struct sockaddr *)NULL, NULL); //在进入监听的前提下, 等待连接
    printf("Get the Client.\n");

    int msgLen = send(clntSock,"Welcome to my server\n", 21, 0); //发送测试数据
    while(1)
    {
        printf("Waiting Clinet msg...\n");
        msgLen = recv(clntSock, buff, BUFSIZ, 0); //接受数据
        buff[msgLen] = '\0';
        time(&time_now);
        printf("[%ld]client: %s\n", time_now, buff);

        printf("Entersome thing:");
        memset(sendBUFF, 0, sizeof(sendBUFF));
        scanf("%s",sendBUFF);
        if (strcmp(sendBUFF,"quit") == 0) { //quit校验
            break;
        }
        send(clntSock, sendBUFF, sizeof(sendBUFF), 0);//发送数据
    }

    printf("*** Quit Server ***\n");
    close(clntSock);
    close(servSocket);
    return 0;
}
