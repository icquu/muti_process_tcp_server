#include <stdio.h>
#include <sys/epoll.h>
#include <netinet/in.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

int doing = 1;

int main() {
    struct sockaddr_in serveraddr, clientaddr;
    int ret, flag = 1;
    socklen_t len = sizeof(serveraddr);
    int lfd = socket(AF_INET, SOCK_STREAM, 0);
    if (lfd < 0) {
        perror("socket");
        exit(1);
    }
    setsockopt(lfd, SOL_SOCKET, SO_REUSEADDR, &flag, sizeof(int));

    bzero(&serveraddr, sizeof(serveraddr));
    serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serveraddr.sin_port = htons(22);
    serveraddr.sin_family = AF_INET;

    ret = bind(lfd, (const struct sockaddr *) &serveraddr, len);
    if (ret < 0) {
        perror("bind");
        exit(1);
    }
    listen(lfd, 128);

    int ep_fd = epoll_create(128);
    struct epoll_event event, events[128];
    event.data.fd = lfd;
    event.events = EPOLLIN;
    epoll_ctl(ep_fd, EPOLL_CTL_ADD, lfd, &event);
    char buf[BUFSIZ];

    while (doing) {
        int n = epoll_wait(ep_fd, events, 128, -1);
        for (int i = 0; i < n; ++i) {
            if (events[i].data.fd == lfd) {
                if (events[i].events & EPOLLERR) {
                    perror("network error");
                    exit(1);
                } else {
                    int fd = accept(lfd, (struct sockaddr *) &clientaddr, &len);
                    event.data.fd = fd;
                    event.events = EPOLLIN | EPOLLERR | EPOLLRDHUP;
                    epoll_ctl(ep_fd, EPOLL_CTL_ADD, fd, &event);
                }
            } else {
                if (events[i].events & EPOLLERR) {
                    perror("something wrong with the fd");
                    epoll_ctl(ep_fd, EPOLL_CTL_DEL, events[i].data.fd, &events[i]);
                    close(events[i].data.fd);
                    continue;
                } else if (events[i].events & EPOLLRDHUP) {
                    printf("client have disconnected\n");
                    epoll_ctl(ep_fd, EPOLL_CTL_DEL, events[i].data.fd, &events[i]);
                    close(events[i].data.fd);
                    continue;
                } else {
                    memset(buf, '\0', BUFSIZ);
                    read(events[i].data.fd, buf, BUFSIZ);
                    printf("%s", buf);
                }
            }
        }
    }

    return 0;
}
