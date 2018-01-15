#include <stdio.h>
#include <getopt.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <time.h>

#include "pc_stencil/core.h"
#include "pc_stencil/sequential.h"
#include "pc_stencil/openmp.h"
#include "pc_stencil/cilk_stencil.h"

#define MAX_FILENAME_CHARS (256)

enum PROC_MODE {
    PROC_MODE_UNDEF = 0,
    NAIVE = 'N',
    SEQUENTIAL = 'S',
    OPENMP = 'O',
    CILK = 'C',
    MPI = 'M'
};

enum STENCIL_MODE {
    STENCIL_UNDEF = 0,
    STENCIL_7 = 7,
    STENCIL_27 = 27
};

void naive_mode(struct pc_matrix *matrix, enum STENCIL_MODE, struct timespec *start, struct timespec *end);
void parallel_mode(struct pc_matrix *matrix, enum STENCIL_MODE, struct timespec *start, struct timespec *end);
void sequential_mode(struct pc_matrix *matrix, enum STENCIL_MODE, struct timespec *start, struct timespec *end);
void openmp_mode(struct pc_matrix *matrix, enum STENCIL_MODE stencil_mode, struct timespec *start, struct timespec *end);
void cilk_mode(struct pc_matrix *matrix, enum STENCIL_MODE stencil_mode, struct timespec *start, struct timespec *end);
void print_usage();
void exit_error(char *msg);
void print_time(FILE *f, uint64_t value) ;

int main(int argc, char **argv) {
    int c;
    bool verbose = false;
    bool check_correctness = false;
    bool print_timing = false;
    char *input_filename = NULL;
    char *output_filename = NULL;
    enum PROC_MODE proc_mode = PROC_MODE_UNDEF;
    enum STENCIL_MODE stencil_mode = STENCIL_UNDEF;

    while ((c = getopt(argc, argv, "chi:m:o:sStv")) != -1) {
        switch (c) {
            case 'c':
                check_correctness = true;
                break;
            case 'i':
                input_filename = optarg;
                break;
            case 'm':
                proc_mode = (enum PROC_MODE) *optarg;
                if (proc_mode != NAIVE
                    && proc_mode != SEQUENTIAL
                    && proc_mode != OPENMP
                    && proc_mode != CILK
                    && proc_mode != MPI) {
                    exit_error("Unknown process mode specifier (-p)!");
                }
                break;
            case 'o':
                output_filename = optarg;
                break;
            case 's':
                if (stencil_mode != STENCIL_UNDEF)
                    exit_error("Stencil mode must be either -s (7 stencil) or -S (27 stencil)!");

                stencil_mode = STENCIL_7;
                break;
            case 'S':
                if (stencil_mode != STENCIL_UNDEF)
                    exit_error("Stencil mode must be either -s (7 stencil) or -S (27 stencil)!");

                stencil_mode = STENCIL_27;
                break;
            case 't':
                print_timing = true;
                break;
            case 'v':
                verbose = true;
                break;
            case 'h':
            case '?':
            default:
                print_usage();
                exit(-1);
        }
    }

    if (proc_mode == PROC_MODE_UNDEF)
        exit_error("No process mode (-p) specified!");

    if (stencil_mode == STENCIL_UNDEF)
        exit_error("No stencil mode (-s|-S) specified!");

    struct pc_matrix matrix = parse(input_filename);

    switch (parcomp_parser_error) {
        case 1:
            exit_error("Can't open input file!");
            break;
        case 2:
            exit_error("Input file is empty!");
            break;
        case 4:
            exit_error("Could not parse x,y,z!");
            break;
        case 8:
            exit_error("Line count does not equal x*y*z!");
        default:
            if (verbose) printf("The file was parsed successfully.\n");
    }

    struct pc_matrix comparison_matrix;

    if (check_correctness) {
        if (verbose) printf("Duplicating input matrix for correctness check...\n");

        comparison_matrix.x = matrix.x;
        comparison_matrix.y = matrix.y;
        comparison_matrix.z = matrix.z;

        init_matrix(&comparison_matrix);
        alloc_matrix(&comparison_matrix);

        copy_matrix(&matrix, &comparison_matrix);

        if (verbose) printf("Matrix duplicated.\n");
    }

    if (verbose) printf("Computing input matrix in mode: ");

    struct timespec t_start, t_end;

    switch (proc_mode) {
        case NAIVE:
            if (verbose) printf("naive...\n");
            naive_mode(&matrix, stencil_mode, &t_start, &t_end);
            break;
        case SEQUENTIAL:
            if (verbose) printf("sequential...\n");
            sequential_mode(&matrix, stencil_mode, &t_start, &t_end);
            break;
        case OPENMP:
            if (verbose) printf("OpenMP...\n");
            openmp_mode(&matrix, stencil_mode, &t_start, &t_end);
            break;
        case CILK:
            if (verbose) printf("cilk...\n");
            cilk_mode(&matrix, stencil_mode, &t_start, &t_end);
            break;
        case MPI:
            if (verbose) printf("MPI...\n");
            exit_error("MPI not implemented!");
            break;
    }

    uint64_t timde_diff = (uint64_t) 1000000000L * (t_end.tv_sec - t_start.tv_sec) + t_end.tv_nsec - t_start.tv_nsec;

    if (verbose) printf("Finished computation.\n");

    if (verbose || print_timing) {
        printf("Time:\n");
        switch (proc_mode) {
            case NAIVE:
                printf("Algorithm: naive ");
                break;
            case SEQUENTIAL:
                printf("Algorithm: sequential ");
                break;
            case OPENMP:
                printf("Algorithm: OpenMP ");
                break;
            case CILK:
                printf("Algorithm: cilk ");
                break;
            case MPI:
                printf("Algorithm: MPI ");
                break;
        }
        print_time(stdout, timde_diff);
    }

    if (check_correctness) {
        if (verbose) printf("Computing matrix values for correctness check...\n");

        sequential_mode(&comparison_matrix, stencil_mode, &t_start, &t_end);

        if (verbose) printf("Finished computation.\n");
        if (verbose) printf("Computing differences in matrices...\n");

        int equal = equal_matrix(&comparison_matrix, &matrix, CMP_THRESHOLD);

        if (equal == 0) {
            exit_error("Output of computed matrix is incorrect!");
        } else if (verbose) {
            printf("No differences found! The output seems correct.\n");
        }

        // IF WE NEED TO PRINT A SOLUTION.. UNCOMMENT THIS
        //FILE *fp = fopen("out_put.txt", "a");
        //print_matrix(fp, &matrix, false);
    }

    if (output_filename != NULL) {
        FILE *f = fopen(output_filename, "a");

        if (f == NULL)
            exit_error("Could not open/create output file!");

        if (verbose) printf("Writing timing results to output file...\n");

        print_time(f, timde_diff);

        if (verbose) printf("Done.\n");
    }

    destroy_matrix(&matrix);

    if (check_correctness)
        destroy_matrix(&comparison_matrix);

    return 0;
}

void naive_mode(struct pc_matrix *matrix, enum STENCIL_MODE stencil_mode, struct timespec *start, struct timespec *end) {
    struct pc_matrix output_matrix;

    output_matrix.x = matrix->x;
    output_matrix.y = matrix->y;
    output_matrix.z = matrix->z;

    init_matrix(&output_matrix);
    alloc_matrix(&output_matrix);

    if (stencil_mode == STENCIL_7) {
        clock_gettime(CLOCK_REALTIME, start);
        run_naive_stencil_7(matrix, &output_matrix);
        clock_gettime(CLOCK_REALTIME, end);
    } else if (stencil_mode == STENCIL_27) {
        clock_gettime(CLOCK_REALTIME, start);
        run_naive_stencil_27(matrix, &output_matrix);
        clock_gettime(CLOCK_REALTIME, end);
    }

    double **tmp = matrix->arr;
    matrix->arr = output_matrix.arr;
    output_matrix.arr = tmp;

    destroy_matrix(&output_matrix);
}

void sequential_mode(struct pc_matrix *matrix, enum STENCIL_MODE stencil_mode, struct timespec *start, struct timespec *end) {
    if (stencil_mode == STENCIL_7) {
        clock_gettime(CLOCK_REALTIME, start);
        run_stencil_7(matrix);
        clock_gettime(CLOCK_REALTIME, end);
    } else if (stencil_mode == STENCIL_27) {
        clock_gettime(CLOCK_REALTIME, start);
        run_stencil_27(matrix);
        clock_gettime(CLOCK_REALTIME, end);
    }
}


void openmp_mode(struct pc_matrix *matrix, enum STENCIL_MODE stencil_mode, struct timespec *start, struct timespec *end) {
    if (stencil_mode == STENCIL_7) {
        clock_gettime(CLOCK_REALTIME, start);
        run_openmp_stencil_7(matrix);
        clock_gettime(CLOCK_REALTIME, end);
    } else if (stencil_mode == STENCIL_27) {
        clock_gettime(CLOCK_REALTIME, start);
        run_openmp_stencil_27(matrix);
        clock_gettime(CLOCK_REALTIME, end);
    }
}

void cilk_mode(struct pc_matrix *matrix, enum STENCIL_MODE stencil_mode, struct timespec *start, struct timespec *end) {
    if (stencil_mode == STENCIL_7) {
        clock_gettime(CLOCK_REALTIME, start);
        run_cilk_stencil_7(matrix);
        clock_gettime(CLOCK_REALTIME, end);
    } else if (stencil_mode == STENCIL_27) {
        fprintf(stderr, "27 stencil not yet implemented for cilk!\n");
    }
}

void print_usage() {
    fprintf(stderr, "Impl print_usage()!");
}

void exit_error(char *msg) {
    fprintf(stderr, "Error: %s\n", msg);
    print_usage();
    exit(-1);
}

void print_time(FILE *f, uint64_t value) {
    int nano = (int) value % 1000;
    value /= 1000;
    int micro = (int) value % 1000;
    value /= 1000;
    int milli = (int) value % 1000;
    value /= 1000;
    int seconds = (int) (value % 60);
    value /= 60;
    int minutes = (int) (value % 60);

    fprintf(f, "%dm %ds %dms %dµs %dns\n", minutes, seconds, milli, micro, nano);
}