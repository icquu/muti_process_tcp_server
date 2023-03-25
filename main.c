#include <stdio.h>
#include <netinet/in.h>
#include <strings.h>
#include <string.h>
#include <stdlib.h>
#include <zconf.h>
#include <signal.h>
#include <wait.h>
#include <arpa/inet.h>

#define PORT 6767

void handler(int arg){
    waitpid(0,NULL,0);
}

int main(int argc, char **argv) {
    signal(SIGCHLD,handler);
    struct sockaddr_in serveraddr, clientaddr;
    bzero(&serveraddr, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serveraddr.sin_port = htons(PORT);
    int lfd = socket(AF_INET, SOCK_STREAM, 0);
    if (lfd == -1) {
        perror("socket");
        exit(1);
    }
    int ret = 0;
    int flag = 1;
    setsockopt(lfd, SOL_SOCKET, SO_REUSEADDR, &flag, sizeof(flag));
    ret = bind(lfd,(const struct sockaddr *)&serveraddr, sizeof(serveraddr));
    if (ret<0){
        perror("bind");
        exit(1);
    }
    listen(lfd, 128);
    int length = sizeof(serveraddr), cfd;
accept_label:
    cfd = accept(lfd,(struct sockaddr*)&clientaddr,&length);
    printf("%s:%u connected.\n", inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));
    pid_t pt = fork();
    if (pt<0){
        perror("fork");
        exit(1);
    }
    if (pt>0){
        close(cfd);
        goto accept_label;
    }

    if (pt==0){
        close(lfd);
        char buf[BUFSIZ]={'\0'};
        int read_ret = 0;
        while ((read_ret = read(cfd, buf, BUFSIZ)) > 0){
            printf("%.2s:%u %s", inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port)%200, buf);
            memset(buf, '\0', read_ret);
        }
        printf("%s:%u disconnected.\n", inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));
        close(cfd);
    }

#if 0
    float start_num = 0.00001;
    for (; start_num < 1; start_num += 0.1) {
        printf("%.15f\n", start_num);
    }
    printf("End\n");
#endif
    return 0;
}
