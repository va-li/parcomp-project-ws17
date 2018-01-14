#include <stdio.h>
#include <getopt.h>
#include <stdlib.h>
#include <stdbool.h>

#include "pc_stencil/core.h"
#include "pc_stencil/sequential.h"
#include "pc_stencil/openmp.h"
#include "pc_stencil/cilk_stencil.h"

#define MAX_FILENAME_CHARS (256)

enum PROC_MODE {
    NAIVE = 0,
    SEQUENTIAL,
    OPENMP,
    CILK,
    MPI
};

enum STENCIL_MODE {
    STENCIL_7 = 0,
    STENCIL_27
};

void naive_mode(struct pc_matrix *matrix, enum STENCIL_MODE);
void parallel_mode(struct pc_matrix *matrix, enum STENCIL_MODE);
void sequential_mode(struct pc_matrix *matrix, enum STENCIL_MODE);
void openmp_mode(struct pc_matrix *matrix, enum STENCIL_MODE stencil_mode);
void cilk_mode(struct pc_matrix *matrix, enum STENCIL_MODE stencil_mode);
void print_usage();

int main(int argc, char **argv) {
    int c;
    char *input_filename = "input.txt";
    bool debug = false, print = true;
    enum PROC_MODE proc_mode = NAIVE;
    enum STENCIL_MODE stencil_mode = STENCIL_7;

    while ((c = getopt(argc, argv, "dhi:m:p:s")) != -1) {
        switch (c) {
            case 'd':
                debug = true;
                break;
            case 'i':
                input_filename = optarg;
                break;
            case 'p':
                proc_mode = (enum PROC_MODE) strtol(optarg, NULL, 10);
                if (proc_mode < NAIVE
                    || proc_mode > MPI) {
                    print_usage();
                    return -1;
                }
                break;
            case 'm':
                stencil_mode = (enum STENCIL_MODE) strtol(optarg, NULL, 10);
                if (stencil_mode < STENCIL_7
                    || stencil_mode > STENCIL_27) {
                    print_usage();
                    return -1;
                }
                break;
            case 's':
                print = false;
                break;
            case 'h':
            case '?':
            default:
                print_usage();
                return -1;
        }
    }

    struct pc_matrix matrix = parse(input_filename);

    switch (parcomp_parser_error) {
        case 1:
            printf("Error: can't open the file.\n");
            break;
        case 2:
            printf("Error: file is empty.\n");
            break;
        case 4:
            printf("Error: could not parse x,y,z.\n");
            break;
        case 8:
            printf("Error: number of lines is not equals x*y*z.\n");
            break;
        default:
            if (print) printf("The file is parsed successfully.\n");
    }

    switch (proc_mode) {
        case NAIVE:
            naive_mode(&matrix, stencil_mode);
            break;
        case SEQUENTIAL:
            sequential_mode(&matrix, stencil_mode);
            break;
        case OPENMP:
            openmp_mode(&matrix, stencil_mode);
            break;
        case CILK:
            cilk_mode(&matrix, stencil_mode);
            break;
        case MPI:

            break;
    }

    if (print) print_matrix(&matrix, false);

    destroy_matrix(&matrix);

    return 0;
}

void naive_mode(struct pc_matrix *matrix, enum STENCIL_MODE stencil_mode) {
    struct pc_matrix output_matrix;

    output_matrix.x = matrix->x;
    output_matrix.y = matrix->y;
    output_matrix.z = matrix->z;

    init_matrix(&output_matrix);
    alloc_matrix(&output_matrix);

    if (stencil_mode == STENCIL_7) {
        run_naive_stencil_7(matrix, &output_matrix);
    } else if (stencil_mode == STENCIL_27) {
        run_naive_stencil_27(matrix, &output_matrix);
    }

    double **tmp = matrix->arr;
    matrix->arr = output_matrix.arr;
    output_matrix.arr = tmp;

    destroy_matrix(&output_matrix);
}

void sequential_mode(struct pc_matrix *matrix, enum STENCIL_MODE stencil_mode) {
    if (stencil_mode == STENCIL_7) {
        run_stencil_7(matrix);
    } else if (stencil_mode == STENCIL_27) {
        run_stencil_27(matrix);
    }
}


void openmp_mode(struct pc_matrix *matrix, enum STENCIL_MODE stencil_mode) {
    if (stencil_mode == STENCIL_7) {
        run_openmp_stencil_7(matrix);
    } else if (stencil_mode == STENCIL_27) {
        run_openmp_stencil_27(matrix);
    }
}

void cilk_mode(struct pc_matrix *matrix, enum STENCIL_MODE stencil_mode) {
    if (stencil_mode == STENCIL_7) {
        run_cilk_stencil_7(matrix);
    } else if (stencil_mode == STENCIL_27) {
        fprintf(stderr, "27 stencil not yet implemented for cilk!\n");
    }
}

void print_usage() {
    fprintf(stderr, "Impl print_usage()!");
}