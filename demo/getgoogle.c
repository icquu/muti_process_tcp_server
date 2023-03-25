#include <stdio.h>
#include <sys/socket.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <time.h>
int main(){

	int fd = socket(AF_INET, SOCK_STREAM, 0);
	struct sockaddr_in serveraddr;

	serveraddr.sin_family = AF_INET;
	inet_aton("172.217.25.4", &serveraddr.sin_addr);
	serveraddr.sin_port = htons(80);

	struct timeval timeout = {1,0};
	setsockopt(fd,SOL_SOCKET,SO_RCVTIMEO,(char *)&timeout,sizeof(struct timeval));
	setsockopt(fd,SOL_SOCKET,SO_SNDTIMEO,(char *)&timeout,sizeof(struct timeval));

	int ret = connect(fd, (const struct sockaddr*)&serveraddr, sizeof(serveraddr));
	if (ret <0){
		perror("connect");
		exit(1);
	}
	char buf[BUFSIZ]={'\0'};
	clock_t start_t, finish_t;
	double last=0;
	start_t = clock();
	write(fd, "GET\n", 4);
	while(read(fd, buf, BUFSIZ)){
//		printf("%s", buf);
	}
	finish_t = clock();
	last = (double)(finish_t - start_t)/CLOCKS_PER_SEC;
	printf("CPU 占用时间 : %f\n", last);

	return 0;
}
