#include <stdio.h>
#include <string.h>

int main(int argc, char **argv){

  FILE *fp = fopen("./test.txt", "r+");
  char str[BUFSIZ] = {'\0'};
  char value[BUFSIZ] = {'\0'};
  while(fgets(str, BUFSIZ, fp) != NULL){
        sscanf(str,"%*[^:]:%*[^:]:%[^:]", value);
        printf("%s\n", value);
        memset(str,'\0',BUFSIZ);
        memset(value,'\0',BUFSIZ);
    }
    
  return 0;
}



