#define _XOPEN_SOURCE
#include <unistd.h>
#include <stdio.h>
#include <crypt.h>
#include <shadow.h>
#include <string.h>

int main(int argc, char **argv){

  puts("模拟Linux验证密码的算法");
  
  char *input_pass = getpass("input your password:");
  
  struct spwd *shdowline = getspnam(argv[1]);
  
  char *crypted_pass = crypt(input_pass,shdowline->sp_pwdp);
  
  if (strcmp(crypted_pass, shdowline->sp_pwdp)==0){
    printf("seccess\n");
  }

  return 0;
}

