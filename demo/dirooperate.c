#include <sys/types.h>
#include <dirent.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>


int main(){
	DIR *dr;
	int fd = open(argv[1], O_RDWR);
	if(fd<0){
		perror("open");
		exit(1);
	}
	dr = fdopendir(fd);
	close(fd);
	return 0;
}
