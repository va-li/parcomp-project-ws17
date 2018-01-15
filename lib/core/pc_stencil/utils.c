#include <stdio.h>
#include "utils.h"

void print_benchmark_human(long microseconds, int xyz, int cores, bool stencil7, char *mode) {
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

void print_benchmark_csv(long microseconds, int xyz, int cores, bool stencil7, char *mode) {
    printf("%dx%dx%d,%d,%s,%d,%ld\n", xyz, xyz, xyz, (stencil7 ? 7 : 27), mode, cores, microseconds);
}

void print_common_usage() {
    fprintf(stderr, "usage: ./pc_stencil_mpi filename [mode]\n");
    fprintf(stderr, "\tonly the first character is checked for the mode\n");
    fprintf(stderr, "Modes:\n");
    fprintf(stderr, "\t\t 1 - Stencil7 csv\n");
    fprintf(stderr, "\t\t 2 - Stencil27 csv\n");
    fprintf(stderr, "\t\t 3 - Stencil7 human readable (default if empty)\n");
    fprintf(stderr, "\t\t 4 - Stencil27 human readable\n");
}
