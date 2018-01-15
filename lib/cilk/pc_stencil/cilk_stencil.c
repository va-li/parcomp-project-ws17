#include <malloc.h>
#include <cilk/cilk.h>
#include <memory.h>
#include "cilk_stencil.h"

#define MIN_SUBPROBLEM_SIZE (20)

#define UPDT_BUFF (buff_arr[z_start])
#define CALC_BUFF (buff_arr[z_start + 1])

static void apply_stencil_7(struct pc_matrix *matrix, int k, double *calc_buff);

static void calc_stencil_7_range(struct pc_matrix *matrix, int z_start, int z_end, double**buff_arr);

static void calc_stencil_27_range(struct pc_matrix *matrix, int z_start, int z_end, double**buff_arr) ;

static void apply_stencil_27(struct pc_matrix *matrix, int k, double *calc_buff) ;

void run_cilk_stencil_7(struct pc_matrix *matrix) {
    //We don't know how often we split, so just make a array of plane buffers the size of Z
    //Only NUMBER_OF_SPLITS * 2 planes are going to be allocated into thi
    double **buff_arr = calloc((size_t)(matrix->z - 2), sizeof(double *));

    for (int pass = 0; pass < ITERATION_COUNT; ++pass) {
        // from the first non-boundary value plane 1 to the last non-boundary value plane matrix->z - 2
        calc_stencil_7_range(matrix, 1, matrix->z - 2, buff_arr);
    }

    for (int k = 0; k < matrix->z - 1; ++k) {
        if (buff_arr[k] != NULL) {
            free(buff_arr[k]);
        }
    }
}

/**
 * Calculate the 7 stencil for planes of MATRIX from Z_START to Z_END and allocate buffer planes to BUFF_ARR for the
 * calculation results if necessary
 * @param matrix
 * @param z_start
 * @param z_end
 * @param buff_arr
 */
static void calc_stencil_7_range(struct pc_matrix *matrix, int z_start, int z_end, double**buff_arr) {
    int z_range = z_end - z_start + 1;
    if (z_range > (MIN_SUBPROBLEM_SIZE * 2)) {
        // This range (subproblem) is not too small to parallelize...

        int z_bot_start;
        int z_bot_end;
        int z_top_start;
        int z_top_end;

        z_bot_start = z_start;
        z_bot_end = (z_range / 2);

        z_top_start = z_bot_end + 2; // not +1 because we leave out one plane in the middle to prevent data races
        z_top_end = z_end;

        // Buffer the predecessor and successor plan to our left out plane
        double *left_out_pred = malloc(matrix->x * matrix->y * sizeof(double));
        double *left_out_succ = malloc(matrix->x * matrix->y * sizeof(double));

        for (int ij = 0; ij < matrix->x * matrix->y; ++ij) {
            left_out_pred[ij] = matrix->arr[z_bot_end][ij];
            left_out_succ[ij] = matrix->arr[z_bot_end + 2][ij];
        }

        // ... so split this range into two and (possibly) parallelize
        cilk_spawn calc_stencil_7_range(matrix, z_bot_start, z_bot_end, buff_arr);
        calc_stencil_7_range(matrix, z_top_start, z_top_end, buff_arr);
        cilk_sync;

        // Insert the buffered planes into the matrix so can correctly perform the stencil on the left out plane
        double *tmp_pred;
        double *tmp_succ;
        tmp_pred = matrix->arr[z_bot_end];
        tmp_succ = matrix->arr[z_bot_end + 2];
        matrix->arr[z_bot_end] = left_out_pred;
        matrix->arr[z_bot_end + 2] = left_out_succ;

        // after both sub ranges calculated, calculate the left out plane
        apply_stencil_7(matrix, z_bot_end + 1, buff_arr[z_bot_start]);

        double *tmp = matrix->arr[z_bot_end + 1];
        matrix->arr[z_bot_end + 1] = buff_arr[z_bot_start];
        buff_arr[z_bot_start] = tmp;

        // Swap the calculated values back in again
        matrix->arr[z_bot_end] = tmp_pred;
        matrix->arr[z_bot_end + 2] = tmp_succ;

        free(left_out_pred);
        free(left_out_succ);

    } else {
        // allocate the calculation and update buffers if this is the first pass over the whole matrix
        if (CALC_BUFF == NULL) {
            CALC_BUFF = malloc(matrix->x * matrix->y * sizeof(double));
            UPDT_BUFF = malloc(matrix->x * matrix->y * sizeof(double));
        }



        for (int k = z_start; k <= z_end; ++k) {

            apply_stencil_7(matrix, k, CALC_BUFF);

            if (k == z_start) {
                double *tmp = CALC_BUFF;
                CALC_BUFF = UPDT_BUFF;
                UPDT_BUFF = tmp;
            } else {
                double *tmp = CALC_BUFF;
                CALC_BUFF = matrix->arr[k - 1];
                matrix->arr[k - 1] = UPDT_BUFF;
                UPDT_BUFF = tmp;
            }
        }

        double *tmp = matrix->arr[z_end];
        matrix->arr[z_end] = UPDT_BUFF;
        UPDT_BUFF = tmp;
    }
}

static void apply_stencil_7(struct pc_matrix *matrix, int k, double *calc_buff) {

    double *pred = matrix->arr[k - 1];
    double *curr = matrix->arr[k];
    double *succ = matrix->arr[k + 1];

    int x = matrix->x;
    int y = matrix->y;

    for (int i = 0; i < x; ++i) {
        ELEMENT(calc_buff, x, i, 0) = ELEMENT(curr, x, i, 0);
        ELEMENT(calc_buff, x, i, y-1) = ELEMENT(curr, x, i, y-1);
    }

    for (int j = 0; j < y; ++j) {
        ELEMENT(calc_buff, x, 0, j) = ELEMENT(curr, x, 0, j);
        ELEMENT(calc_buff, x, x-1, j) = ELEMENT(curr, x, x-1, j);
    }

    for (int j = 1; j < y - 1; ++j) {
        for (int i = 1; i < x - 1; ++i) {

            double tmp = ELEMENT(curr, x, i, j);
            tmp += ELEMENT(curr, x, i, j + 1);
            tmp += ELEMENT(curr, x, i, j - 1);
            tmp += ELEMENT(curr, x, i + 1, j);
            tmp += ELEMENT(curr, x, i - 1, j);
            tmp += ELEMENT(pred, x, i, j);
            tmp += ELEMENT(succ, x, i, j);
            tmp /= 7;
            ELEMENT(calc_buff, x, i, j) = tmp;
        }
    }
}

void run_cilk_stencil_27(struct pc_matrix *matrix) {
    //We don't know how often we split, so just make a array of plane buffers the size of Z
    //Only NUMBER_OF_SPLITS * 2 planes are going to be allocated into thi
    double **buff_arr = calloc((size_t)(matrix->z - 2), sizeof(double *));

    for (int pass = 0; pass < ITERATION_COUNT; ++pass) {
        // from the first non-boundary value plane 1 to the last non-boundary value plane matrix->z - 2
        calc_stencil_27_range(matrix, 1, matrix->z - 2, buff_arr);
    }

    for (int k = 0; k < matrix->z - 1; ++k) {
        if (buff_arr[k] != NULL) {
            free(buff_arr[k]);
        }
    }
}

static void calc_stencil_27_range(struct pc_matrix *matrix, int z_start, int z_end, double**buff_arr) {
    int z_range = z_end - z_start + 1;
    if (z_range > (MIN_SUBPROBLEM_SIZE * 2)) {
        // This range (subproblem) is not too small to parallelize...

        int z_bot_start;
        int z_bot_end;
        int z_top_start;
        int z_top_end;

        z_bot_start = z_start;
        z_bot_end = (z_range / 2);

        z_top_start = z_bot_end + 2; // not +1 because we leave out one plane in the middle to prevent data races
        z_top_end = z_end;

        // Buffer the predecessor and successor plan to our left out plane
        double *left_out_pred = malloc(matrix->x * matrix->y * sizeof(double));
        double *left_out_succ = malloc(matrix->x * matrix->y * sizeof(double));

        for (int ij = 0; ij < matrix->x * matrix->y; ++ij) {
            left_out_pred[ij] = matrix->arr[z_bot_end][ij];
            left_out_succ[ij] = matrix->arr[z_bot_end + 2][ij];
        }

        // ... so split this range into two and (possibly) parallelize
        cilk_spawn calc_stencil_27_range(matrix, z_bot_start, z_bot_end, buff_arr);
        calc_stencil_27_range(matrix, z_top_start, z_top_end, buff_arr);
        cilk_sync;

        // Insert the buffered planes into the matrix so can correctly perform the stencil on the left out plane
        double *tmp_pred;
        double *tmp_succ;
        tmp_pred = matrix->arr[z_bot_end];
        tmp_succ = matrix->arr[z_bot_end + 2];
        matrix->arr[z_bot_end] = left_out_pred;
        matrix->arr[z_bot_end + 2] = left_out_succ;

        // after both sub ranges calculated, calculate the left out plane
        apply_stencil_27(matrix, z_bot_end + 1, buff_arr[z_bot_start]);

        double *tmp = matrix->arr[z_bot_end + 1];
        matrix->arr[z_bot_end + 1] = buff_arr[z_bot_start];
        buff_arr[z_bot_start] = tmp;

        // Swap the calculated values back in again
        matrix->arr[z_bot_end] = tmp_pred;
        matrix->arr[z_bot_end + 2] = tmp_succ;

        free(left_out_pred);
        free(left_out_succ);

    } else {
        // allocate the calculation and update buffers if this is the first pass over the whole matrix
        if (CALC_BUFF == NULL) {
            CALC_BUFF = malloc(matrix->x * matrix->y * sizeof(double));
            UPDT_BUFF = malloc(matrix->x * matrix->y * sizeof(double));
        }

        for (int k = z_start; k <= z_end; ++k) {

            apply_stencil_27(matrix, k, CALC_BUFF);

            if (k == z_start) {
                double *tmp = CALC_BUFF;
                CALC_BUFF = UPDT_BUFF;
                UPDT_BUFF = tmp;
            } else {
                double *tmp = CALC_BUFF;
                CALC_BUFF = matrix->arr[k - 1];
                matrix->arr[k - 1] = UPDT_BUFF;
                UPDT_BUFF = tmp;
            }
        }

        double *tmp = matrix->arr[z_end];
        matrix->arr[z_end] = UPDT_BUFF;
        UPDT_BUFF = tmp;
    }
}

static void apply_stencil_27(struct pc_matrix *matrix, int k, double *calc_buff) {

    double *pred = matrix->arr[k - 1];
    double *curr = matrix->arr[k];
    double *succ = matrix->arr[k + 1];

    int x = matrix->x;
    int y = matrix->y;

    for (int i = 0; i < x; ++i) {
        ELEMENT(calc_buff, x, i, 0) = ELEMENT(curr, x, i, 0);
        ELEMENT(calc_buff, x, i, y-1) = ELEMENT(curr, x, i, y-1);
    }

    for (int j = 0; j < y; ++j) {
        ELEMENT(calc_buff, x, 0, j) = ELEMENT(curr, x, 0, j);
        ELEMENT(calc_buff, x, x-1, j) = ELEMENT(curr, x, x-1, j);
    }

    for (int j = 1; j < y - 1; ++j) {
        for (int i = 1; i < x - 1; ++i) {

            double tmp = 0;
            for (int a = -1; a < 2; ++a) {
                for (int b = -1; b < 2; ++b) {
                    tmp += ELEMENT(pred, x, i + a, j + b);
                    tmp += ELEMENT(curr, x, i + a, j + b);
                    tmp += ELEMENT(succ, x, i + a, j + b);
                }
            }

            tmp /= 27;
            ELEMENT(calc_buff, x, i, j) = tmp;
        }
    }
}
