#include <stdio.h>
#include <signal.h>
#include <zconf.h>
#include <time.h>

int doing = 1;

void handler(int arg) {
    printf("Ctrl+C\n");
    doing = 0;
}

int main() {
    signal(SIGINT, handler);
    time_t seconds;
    struct tm *p;
    char *wday[] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};
    while (doing) {
        seconds = time((time_t *) NULL);
        printf("time: %d\n", seconds);
//      p = gmtime(&seconds);       //UTC时间
        p = localtime(&seconds);    //系统所设置的时区的时间
        printf("%d/%02d/%02d ", (1900 + p->tm_year), (1 + p->tm_mon), p->tm_mday);
        printf("%s %02d:%02d:%02d\n", wday[p->tm_wday], p->tm_hour, p->tm_min, p->tm_sec);
        sleep(1);
    }
    return 0;
}
