#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#include <glob.h>
#include <string.h>

#define PATHSIZE 1024

static int path_noloop(const char *path){

  char *pos;
  pos = strrchr(path,'/');
  if(pos == NULL){
    exit(1);  
  }
  if(strcmp(pos+1,".")==0 || strcmp(pos+1,"..")==0)
    return 0;

  return 1;
}

static int64_t mydu(const char *path){

  //path
  //��Ŀ¼
  int64_t sum=0;
  struct stat statres;
  if(lstat(path,&statres)<0){
    perror("lstat");
    exit(1);
  }
  if(!S_ISDIR(statres.st_mode)){
    return statres.st_blocks;
  }
  
  //Ŀ¼
  //path/*
  //glob();
  char nextpath[PATHSIZE];
  strncpy(nextpath, path, PATHSIZE);
  strncat(nextpath, "/*", PATHSIZE);
  
  glob_t globres;
  glob(nextpath,0,NULL,&globres);
  

  
  //path/.*
  //glob();
  strncpy(nextpath, path, PATHSIZE);
  strncat(nextpath, "/.*", PATHSIZE);
  glob(nextpath,GLOB_APPEND,NULL,&globres);
  
  for(int i=0;i<globres.gl_pathc;i++){
    if(path_noloop(globres.gl_pathv[i]))
      sum+=mydu(globres.gl_pathv[i]);
  }
  sum+=statres.st_blocks;
  return sum;

}


int main(int argc, char **argv){

  if (argc<2){
    printf("usage: %s path\n", argv[0]);
    exit(1);
  }
  
  printf("%lld\n", mydu(argv[1])/2);


  return 0;
}