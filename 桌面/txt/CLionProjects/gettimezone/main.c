#include<time.h>
#include <stdio.h>

int main(int argc, char **argv) {
    unsigned int timezone = 0;
    time_t t1, t2;
    struct tm *tm_local, *tm_utc;
    time(&t1);
    t2 = t1;
    printf("t1=%lu,t2=%lu\n", t1, t2);
    tm_local = localtime(&t1);
    printf("localtime=%d:%d:%d\n", tm_local->tm_hour, tm_local->tm_min, tm_local->tm_sec);
    t1 = mktime(tm_local);
    tm_utc = gmtime(&t2);
    printf("utctime=%d:%d:%d\n", tm_utc->tm_hour, tm_utc->tm_min, tm_utc->tm_sec);
    t2 = mktime(tm_utc);
    printf("*****\n");
    printf("t1=%lu\nt2=%lu\n", t1, t2);
    timezone = (t1 - t2) / 3600;
    printf("%d\n", timezone);
    return 0;
}