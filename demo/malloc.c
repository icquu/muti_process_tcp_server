#include <time.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char **argv){


	clock_t start_t, finish_t;
	double total_t = 0;
	start_t = clock();
	int *p=malloc(1024LL*1024LL*1024LL);
        if (p==NULL){
                printf("malloc failed\n");
                exit(1);
        }
	memset(p, '\0', 1024LL*1024LL*1024LL);
	finish_t = clock();
	total_t = (double)(finish_t - start_t)/CLOCKS_PER_SEC;
	printf("CPU 占用时间 : %f\n", total_t);
//        sleep(10);
        free(p);

        return 0;
}
