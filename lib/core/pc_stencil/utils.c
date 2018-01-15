#include <stdio.h>
#include "utils.h"

void print_benchmark_human(long microseconds, int xyz, int cores, bool stencil7, char* mode) {
    long milli = microseconds / 1000;
    long sec = milli / 1000;
    long min = sec / 60;
    long hr = min / 60;
    milli %= 1000;
    sec %= 60;
    min %= 60;
    printf("Stencil %d on %dx%dx%d matrix. Using %s on %d cores:\n", (stencil7 ? 7 : 27), xyz, xyz, xyz, mode, cores);
    printf("%ldh %ldm %lds %ldmilli\n", hr, min, sec, milli);
}

void print_benchmark_csv(long microseconds, int xyz, int cores, bool stencil7, char* mode) {
    printf("%dx%dx%d,%d,%s,%d,%ld\n",xyz, xyz, xyz, (stencil7?7:27), mode, cores, microseconds);
}