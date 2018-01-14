#include <getopt.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <evdns.h>

#include "pc_stencil/core.h"
#include "pc_stencil/openmp.h"
#include "pc_stencil/sequential.h"

enum STENCIL_MODE {
    STENCIL_7 = 0,
    STENCIL_27
};

void sequential_mode(struct pc_matrix *matrix, enum STENCIL_MODE);

void parallel_mode(struct pc_matrix *matrix, enum STENCIL_MODE);

void print_milli(long value, bool human);

void print_usage() {
    printf("./app [-h] (-B | -C | -BC) (-o | -O | -oO) -m mode -f filename\n");
    printf("-B for benchmarks\n");
    printf("-C for correctness\n");
    printf("-o for stencil7\n");
    printf("-o for stencil27\n");
    printf("filename: should include absolute/relative path if not in the same directory\n");
    printf("mode: should be a bitwise and of the modes\n");
    printf("\t\t00000001 - Sequential\n");
    printf("\t\t00000010 - OpenMP\n");
    printf("\t\t00000100 - Cilk\n");
    printf("\t\t00001000 - MPI\n");
}


int main(int argc, char **argv) {
    int c, tmp;
    char *input_filename = "input.txt";
    bool check_benchmark = false, check_correctness = false, check_stencil7 = false, check_stencil27 = false;
    bool do_sequential = false, do_openmp = false, do_cilk = false, do_mpi = false;
    bool filename_given = false, mode_given = false, human_readable = false;

    while ((c = getopt(argc, argv, "hBCoOm:f:")) != -1) {
        switch (c) {
            case 'h':
                human_readable = true;
                break;
            case 'B':
                check_benchmark = true;
                break;
            case 'C':
                check_correctness = true;
                break;
            case 'o':
                check_stencil7 = true;
                break;
            case 'O':
                check_stencil27 = true;
                break;
            case 'm':
                tmp = (int) strtol(optarg, NULL, 10);
                do_sequential = (tmp & 1) == 1;
                do_openmp = (tmp & 2) == 2;
                do_cilk = (tmp & 4) == 4;
                do_mpi = (tmp & 8) == 8;
                mode_given = true;
                break;
            case 'f':
                input_filename = optarg;
                filename_given = true;
                break;
            default:
                print_usage();
                return -1;
        }
    }

    if (!check_benchmark && !check_correctness) {
        printf("You need to at least ask for benchmarks or correctness.\n");
        print_usage();
        return -1;
    }

    if (!check_stencil7 && !check_stencil27) {
        printf("You need to at least calculate one of the stencils.\n");
        print_usage();
        return -1;
    }

    if (!filename_given) {
        printf("-f should not be empty.\n");
        print_usage();
        return -1;
    }

    if (!mode_given) {
        printf("-m should not be empty\n");
        print_usage();
        return -1;
    }

    int count_modes = 0;
    if (do_sequential) count_modes++;
    if (do_openmp) count_modes++;
    if (do_cilk) count_modes++;
    if (do_mpi) count_modes++;


    if (do_cilk) {
        printf("Cilk mode is not implemented yet!\n");
        count_modes--;
        do_cilk = false;
    }

    if (do_mpi) {
        printf("MPI mode is not implemented yet!\n");
        count_modes--;
        do_mpi = false;
    }


    if (count_modes < 2) {
        printf("You should compare at least two modes -m.\n");
        print_usage();
        return -1;
    }


    struct pc_matrix m_seq = parse(input_filename);
    struct pc_matrix m_omp;

    init_matrix(&m_omp);
    m_omp.x = m_seq.x;
    m_omp.y = m_seq.y;
    m_omp.z = m_seq.z;
    alloc_matrix(&m_omp);
    copy_matrix(&m_seq, &m_omp);


    clock_t begin, end;
    long calc_time = 0;

    if (check_stencil7) {
        printf("%dx%dx%d - Stencil7\n", m_seq.x, m_seq.y, m_seq.z);
        begin = clock();
        sequential_mode(&m_seq, STENCIL_7);
        end = clock();

        if (check_benchmark) {
            printf("\tSequential: \t\t");
            print_milli((long)((end - begin) * 1000 / CLOCKS_PER_SEC), human_readable);
        }

        begin = clock();
        parallel_mode(&m_omp, STENCIL_7);
        end = clock();

        if (check_benchmark) {
            printf("\tOpenMP: \t\t\t");
            print_milli((long)((end - begin) * 1000 / CLOCKS_PER_SEC), human_readable);
        }

        if (check_correctness) {
            int equals = equal_matrix(&m_seq, &m_omp);
            if (equals) {
                printf("The calculation is correct.\n");
            } else {
                printf("The calculation is incorrect.\n");
            }
        }
    } else {
        printf("%dx%dx%d - Stencil27\n", m_seq.x, m_seq.y, m_seq.z);
        begin = clock();
        sequential_mode(&m_seq, STENCIL_27);
        end = clock();

        if (check_benchmark) {
            printf("\tSequential: \t\t");
            print_milli((long)((end - begin) * 1000 / CLOCKS_PER_SEC), human_readable);
        }

        begin = clock();
        parallel_mode(&m_omp, STENCIL_27);
        end = clock();

        if (check_benchmark) {
            printf("\tOpenMP: \t\t\t");
            print_milli((long)((end - begin) * 1000 / CLOCKS_PER_SEC), human_readable);
        }

        if (check_correctness) {
            int equals = equal_matrix(&m_seq, &m_omp);
            if (equals) {
                printf("The calculation is correct.\n");
            } else {
                printf("The calculation is incorrect.\n");
            }
        }
    }


    return 0;
}

void print_milli(long value, bool human) {
    if (!human) {
        printf("\t%d milliseconds\n", value);
        return;
    }

    int milli = (int) value % 1000;
    value /= 1000;
    int seconds = (int) (value % 60);
    value /= 60;
    int minutes = (int) (value % 60);
    value /= 60;
    int hours = (int) value;

    printf("%dh %dm %ds %d\n", hours, minutes, seconds, milli);
}


void sequential_mode(struct pc_matrix *matrix, enum STENCIL_MODE stencil_mode) {
    if (stencil_mode == STENCIL_7) {
        run_stencil_7(matrix);
    } else if (stencil_mode == STENCIL_27) {
        run_stencil_27(matrix);
    }
}

void parallel_mode(struct pc_matrix *matrix, enum STENCIL_MODE stencil_mode) {
    if (stencil_mode == STENCIL_7) {
        run_openmp_stencil_7(matrix);
    } else if (stencil_mode == STENCIL_27) {
        run_openmp_stencil_27(matrix);
    }
}

