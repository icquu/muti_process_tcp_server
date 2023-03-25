#include <stdlib.h>
#include <stdio.h>

int main() {
    FILE *fp = fopen("test.txt", "w");
    if (fp==NULL){
        perror("fopen");
        exit(1);
    }
    fprintf(fp,"hello world\n");
    fclose(fp);

    fp = fopen("test.txt", "r+");
    if (fp==NULL){
        perror("fopen");
        exit(1);
    }
    char buf[BUFSIZ]={'\0'};
    fgets(buf,BUFSIZ,fp);
//    fscanf(fp,"%s",buf);
    printf("%s\n", buf);
    fclose(fp);
    return 0;
}
