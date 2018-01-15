#include <stdio.h>
#include <stdbool.h>
#include <time.h>

#include "pc_stencil/core.h"
#include "pc_stencil/utils.h"
#include "pc_stencil/cilk_stencil.h"

void print_benchmark(long nanoseconds, int xyz, int cores, bool stencil7, bool human) {
    xyz -= 2;

    if (human) print_benchmark_human(nanoseconds / 1000, xyz, cores, stencil7, "Cilk");
    else print_benchmark_csv(nanoseconds / 1000, xyz, cores, stencil7, "Cilk");
}

int main(int argc, char **argv) {
    bool stencil7 = true, human = true, print_out = false;

    if (argc < 2 || argc > 3) {
        print_common_usage();
        return -1;
    }

    if (argc == 3)
        switch (argv[2][0]) {
            case '1':
                stencil7 = true;
                human = false;
                break;
            case '2':
                stencil7 = false;
                human = false;
                break;
            case '3':
                stencil7 = true;
                human = true;
                break;
            case '4':
                stencil7 = false;
                human = true;
                break;
            default:
                print_common_usage();
                return -1;
        }

    if (argv[2][1] == '+') {
        print_out = true;
    }

    struct pc_matrix matrix = parse(argv[1]);

    if (matrix.arr == (void *) 0) {
        fprintf(stderr, "Could not parse matrix from file %s\n", argv[1]);
        return -1;
    }

    struct timespec start, end;

    clock_gettime(0, &start);

    if (stencil7) run_cilk_stencil_7(&matrix);
    else run_cilk_stencil_27(&matrix);

    clock_gettime(0, &end);

    if (print_out) {
        print_matrix(stdout, &matrix, false);
    } else {
        print_benchmark(((long)end.tv_sec*1000000000 + end.tv_nsec) - ((long)start.tv_sec*1000000000 + start.tv_nsec), matrix.x, pc_openmp_used_threads, stencil7, human);
    }


    return 0;
}