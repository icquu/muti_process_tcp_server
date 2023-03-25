#include <stdio.h>
#include <stdlib.h>
#include <setjmp.h>
jmp_buf save;
int a(void){
	printf("I'm a function!\n");
	return 0;
}

int b(void){
  longjmp(save,5);
  a();
	printf("I'm b function!\n");
	return 0;
}

int c(void){
  b();
	printf("I'm c function!\n");
	return 0;
}

int main(int argc, char **argv){
	printf("%s():Begin.\n", __FUNCTION__);
	printf("%s():Call c().\n", __FUNCTION__);
	printf("%s():c() returned.\n", __FUNCTION__);

  int ret = setjmp(save);
  if (ret ==0){
    printf("make a longjmp savepoint\n");
   	c();
  }else{
    printf("main function ended.\n");
  }
  
	exit(0);
}
