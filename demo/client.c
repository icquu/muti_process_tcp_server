#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <time.h>

#define SOCEKT_FAILL (-1)

int main()
{
    int clntSocket;
    char buf[BUFSIZ];//BUFSIZ = 8192
    struct sockaddr_in clntAddr;
    time_t time_now;
    int on = 1;

    printf("*** Client Ｓtart! ***\n");

    while((clntSocket = socket(AF_INET,SOCK_STREAM,0)) == SOCEKT_FAILL);
    printf("Create Client Socket Success!\n");

    setsockopt(clntSocket, SOL_SOCKET,SO_REUSEADDR, &on, sizeof(on));

    memset(&clntAddr, 0, sizeof(clntAddr));
    clntAddr.sin_family = AF_INET;
    clntAddr.sin_addr.s_addr = INADDR_ANY;
    clntAddr.sin_port = htons(8899);

    while(connect(clntSocket, (struct sockaddr*)&clntAddr, sizeof(struct sockaddr)) == SOCEKT_FAILL);
    printf("Connect Socket Success!\n");

    int msgLen = recv(clntSocket, buf, BUFSIZ,0);
    buf[msgLen]='\0';
    printf("%s",buf);

    while(1)
    {
        printf("Enter Words:");
        scanf("%s",buf);

        if (strcmp(buf,"quit") == 0) {
            break;
        }
        send(clntSocket, buf, strlen(buf), 0);

        printf("Waiting Server Msg...\n");
        msgLen = recv(clntSocket, buf, BUFSIZ,0);
        buf[msgLen]='\0';
        time(&time_now);
        printf("[%ld]server: %s\n", time_now,buf);
    }

    printf("*** Quit Clinet ***\n");
    close(clntSocket);
    return 0;
}
