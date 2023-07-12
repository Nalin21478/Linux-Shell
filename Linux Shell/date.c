#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

void print_local_time() {
    time_t t = time(NULL);
    struct tm *timeInfo = localtime(&t);
    char printableTime[100];
    strftime(printableTime, sizeof(printableTime), "%A %d %B %Y %l:%M:%S %p %Z", timeInfo);
    printf("%s\n", printableTime);
}

void print_utc_time() {
    time_t t = time(NULL);
    struct tm *timeInfo = gmtime(&t);
    char printableTime[100];
    strftime(printableTime, sizeof(printableTime), "%A %d %B %Y %l:%M:%S %p %Z", timeInfo);
    printf("%s\n", printableTime);
}

void print_rfc_time() {
    time_t t = time(NULL);
    struct tm *timeInfo = localtime(&t);
    char printableTime[100];
    strftime(printableTime, sizeof(printableTime), "%a, %d %b %Y %H:%M:%S %z", timeInfo);
    printf("%s\n", printableTime);
}

int main(int argc, char *argv[]) {
    if (argc == 1) {
        print_local_time();
    } else if (argc > 2) {
        printf("Excess arguments received\n");
    } else if (strcmp(argv[1], "-u") == 0) {
        print_utc_time();
    } else if (strcmp(argv[1], "-R") == 0) {
        print_rfc_time();
    } else {
        printf("Invalid argument received\n");
    }

    return 0;
}
