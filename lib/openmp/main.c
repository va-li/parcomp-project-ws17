#include <stdio.h>
#include <stdbool.h>
#include <omp.h>
#include <string.h>

#include "pc_stencil/core.h"
#include "pc_stencil/utils.h"
#include "pc_stencil/openmp.h"

void print_benchmark(double seconds, int xyz, int cores, bool stencil7, bool human) {
    long sec = (long) seconds;
    long micro = (long) ((seconds - sec) * 1000000);
    micro += sec * 1000000;

    xyz -= 2;

    if (human) print_benchmark_human(micro, xyz, cores, stencil7, "OpenMP");
    else print_benchmark_csv(micro, xyz, cores, stencil7, "OpenMP");
}

int main(int argc, char **argv) {
    bool stencil7 = true, human = true, print_out = false;

    if (argc < 2 || argc > 3) {
        print_common_usage();
        fprintf(stderr, "ADD (+n | -n) IN MODE PARAMETER (+ print out, - dont print out, n = number of suggested processors) ");
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
                fprintf(stderr, "ADD (+n | -n) IN MODE PARAMETER (+ print out, - dont print out, n = number of suggested processors) ");
                return -1;
        }

    if (argv[2][1] == '+') {
        print_out = true;
    }

    if (strlen(argv[2]) >= 3) {
        int core_count = 0;
        char *it = &argv[2][2];
        while (*it != '\n' && *it >= '0' && *it <= '9') {
            core_count *= 10;
            core_count += *it - '0';
            *it++;
        }
        if (core_count > 0) pc_openmp_sugessted_threads = core_count;
    }

    struct pc_matrix matrix = parse(argv[1]);

    if (matrix.arr == (void *) 0) {
        fprintf(stderr, "Could not parse matrix from file %s\n", argv[1]);
        return -1;
    }

    double start = 0, end = 0;

    start = omp_get_wtime();

    if (stencil7) run_openmp_stencil_7(&matrix);
    else run_openmp_stencil_27(&matrix);

    end = omp_get_wtime();

    if (print_out) {
        print_matrix(stdout, &matrix, false);
    } else {
        print_benchmark(end - start, matrix.x, pc_openmp_used_threads, stencil7, human);
    }


    return 0;
}