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
    NAIVE = 'N',
    SEQUENTIAL = 'S',
    OPENMP = 'O',
    CILK = 'C',
    MPI = 'M'
};

enum STENCIL_MODE {
    STENCIL_7 = 7,
    STENCIL_27 = 27
};

void naive_mode(struct pc_matrix *matrix, enum STENCIL_MODE);
void parallel_mode(struct pc_matrix *matrix, enum STENCIL_MODE);
void sequential_mode(struct pc_matrix *matrix, enum STENCIL_MODE);
void openmp_mode(struct pc_matrix *matrix, enum STENCIL_MODE stencil_mode);
void cilk_mode(struct pc_matrix *matrix, enum STENCIL_MODE stencil_mode);
void print_usage();
void exit_error(const char *msg);

int main(int argc, char **argv) {
    int c;
    bool verbose = false;
    char *input_filename = NULL;
    char *output_filename = NULL;
    enum PROC_MODE proc_mode = NULL;
    enum STENCIL_MODE stencil_mode = NULL;

    while ((c = getopt(argc, argv, "hi:o:sSv")) != -1) {
        switch (c) {
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
            case 's':
                if (stencil_mode != NULL)
                    exit_error("Stencil mode must be either -s (7 stencil) or -S (27 stencil)");

                stencil_mode = STENCIL_7;
                break;
            case 'S':
                if (stencil_mode != NULL)
                    exit_error("Stencil mode must be either -s (7 stencil) or -S (27 stencil)");

                stencil_mode = STENCIL_27;
                break;
            case 'o':
                output_filename = optarg;
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

    if (proc_mode == NULL)
        exit_error("No process mode (-p) specified!");

    if (stencil_mode == NULL)
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

    if (output_filename != NULL) {
        FILE *f = fopen(output_filename, "w");

        if (f == NULL)
            exit_error("Could not open/create output file!");

        print_matrix(f, &matrix, false);
    }

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

void exit_error(const char *msg) {
    fprintf(stderr, "Error: %s\n");
    print_usage();
    exit(-1);
}