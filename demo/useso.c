#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#define _GNU_SOURCE
#include <dlfcn.h>
#include <sys/mman.h>
#include <string.h>

typedef int (*FUNC_PTR)(int);

FUNC_PTR p_func;

void *lib;

void sigfun(int arg){
    //1. 调用dlopen加载so库
    char patch[] = "./libmydy.so";
    dlclose(lib);
    lib = dlopen(patch, RTLD_NOW);
    if (NULL == lib)
    {
        printf("dlopen failed , %s\n", patch);
        return;
    }
    // 2. 查找函数符号表并且替换
    p_func = (FUNC_PTR)dlsym(lib, "print_page");
    if (NULL == p_func)
    {
        printf("fix symbol failed: %s\n", dlerror());
        dlclose(lib);
        return;
    }
    
    return;
}

int main(int argc, char **argv){
  signal(SIGUSR1, sigfun);
  char patch[] = "./libmydy.so";
  lib = dlopen(patch, RTLD_NOW);
  if (NULL == lib)
  {
    printf("dlopen failed , %s\n", patch);
    return 1;
  }
  
  // 2. 查找函数符号表并且替换
  p_func = (FUNC_PTR)dlsym(lib, "print_page");
  if (NULL == p_func)
  {
    printf("fix symbol failed");
    dlclose(lib);
    return 1;
  }

  int a=1;

  while(1){
    p_func(a);
    sleep(1);
  }
  return 0;
}
