#include <stdio.h>
#include <string.h>
#include <sys/stat.h>

int main() {
    umask(0);
    FILE *fp = fopen("./unencrypt_file", "w+");
    fprintf(fp, "Mr right");
    fclose(fp);

    fp = fopen("./unencrypt_file", "r+");
    FILE* rfp=fopen("./encrypt_file", "w+");
    char sn[] = "jjkijjkkjj";
    char buf[BUFSIZ];
    while (fgets(buf, BUFSIZ, fp)){
        for (int i = 0; i < strlen(buf); ++i) {
            printf("%c,%c,%c,%c\n", buf[i],sn[i%strlen(sn)],buf[i]^sn[i%strlen(sn)],buf[i]^sn[i%strlen(sn)]^sn[i%strlen(sn)]);
            fputc(buf[i]^sn[i%strlen(sn)],rfp);
        }
    }
    fclose(fp);
    fclose(rfp);
    return 0;
}
