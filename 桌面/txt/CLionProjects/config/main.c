#include <stdio.h>
#include <sys/stat.h>
#include <string.h>

/*
 * 读写配置文件
 */

typedef struct configinfo {
    char key[64];
    char value[64];
} ConfigInfo;

int main() {
    ConfigInfo conf[BUFSIZ];
    int index = 0;
    umask(0);
    char buf[BUFSIZ];
    FILE *fp = fopen("config.txt", "r+");
    while(fgets(buf, BUFSIZ, fp)){
        sscanf(buf, "%[a-z]:%s", conf[index].key, conf[index].value);
        printf("%s %s\n", conf[index].key, conf[index].value);
        index++;
        memset(buf,'\0', BUFSIZ);
    }
    printf("%d\n", index);
    fclose(fp);

    memcpy(buf,"English learning",16);
    printf("%s\n", strstr(buf,"sh"));
    sprintf(buf,"%s\n", conf[0].key);
    puts(buf);
//    printf sprintf fprintf
//    scanf sscanf fscanf

    puts("长津湖15W中4W冻死，但彭总后来却被毛泽东当作右倾靶子，进行了政治批斗了，\n"
         "中共的浮夸之风，形式主义，官僚主义愈演愈烈，这样的环境下，需要认认真真，\n"
         "踏踏实实做的事情，在中共体制下，是没有发展土壤的，比如：芯片，材料科学，\n"
         "大飞机，关键部件等等。\n");
    puts("有一座座丰碑在民族的历史上，人们忘却了那本来一条条鲜活的生命。\n"
         "三权分立，是防止独裁的一个措施。\n"
         "基础科学研究方面，美国仍然是全球领先的，中国的基础科研仍非常落后。\n"
         "Can you really tell if a kid is lying? "
         "Now, we can use some science to help us to analyze\n"
         "Giving Harvard Students an iPhone 11 If They Can Answer These Questions"
         "How I got to Google from being a factory worker");

    return 0;
}