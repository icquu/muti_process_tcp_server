#include <time.h>
#include <stdio.h>

int main(int argc, char **argv){

  time_t seconds;
  
  seconds = time(NULL);
  printf("自 1970-01-01 起的小时数 = %ld\n", seconds/3600);
  
  //localtime()
  struct tm *Now;
  Now = gmtime(&seconds);
  printf("UTC时间: %s", asctime(Now));
  Now = localtime(&seconds);
  printf("系统时区时间: %s", asctime(Now));
  printf("当前时间 = %s", ctime(&seconds));
  
  return 0;
}