#include <time.h>
#include <stdio.h>

int main(int argc, char **argv){

  clock_t start_t, finish_t;
  double total_t = 0;
  int i = 0;
  start_t = clock();
  for(;i<1000000000;i++){
    
  }
  finish_t = clock();
  total_t = (double)(finish_t - start_t)/CLOCKS_PER_SEC;
  printf("CPU 占用时间 : %f\n", total_t);
  return 0;
}
