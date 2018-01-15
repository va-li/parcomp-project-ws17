#include <malloc.h>
#include <cilk/cilk.h>
#include <memory.h>
#include "cilk_stencil.h"

#define MIN_SUBPROBLEM_SIZE (20)

static void apply_stencil_7(struct pc_matrix *matrix, int k, double *calc_buff);

static void calc_stencil_7_range(struct pc_matrix *matrix, int z_start, int z_end, double**buff_arr);

void run_cilk_stencil_7(struct pc_matrix *matrix) {
    //We don't know how often we split, so just make a array of plane buffers the size of Z
    //Only NUMBER_OF_SPLITS * 2 planes are going to be allocated into thi
    double **buff_arr = malloc((matrix->z - 2) * sizeof(double *));

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
    int z_range = z_end - z_start;
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

        // ... so split this range into two and (possibly) parallelize
        cilk_spawn calc_stencil_7_range(matrix, z_bot_start, z_bot_end, buff_arr);
        calc_stencil_7_range(matrix, z_top_start, z_top_start, buff_arr);
        cilk_sync;

        // after both sub ranges calculated, calculate the left out plane
        apply_stencil_7(matrix, z_bot_end + 1, buff_arr[z_bot_start]);

        double *tmp = matrix->arr[z_bot_end + 1];
        matrix->arr[z_bot_end + 1] = buff_arr[z_bot_start];
        buff_arr[z_bot_start] = tmp;

    } else {
        // This range is too small to parallelize
        double *updt_buff;
        double *calc_buff;
        // allocate the calculation and update buffers if this is the first pass over the whole matrix
        if (buff_arr[z_start] == NULL) {
            buff_arr[z_start] = updt_buff = malloc(matrix->x * matrix->y * sizeof(double));
            buff_arr[z_start + 1] = calc_buff = malloc(matrix->x * matrix->y * sizeof(double));
        } else {
            updt_buff = buff_arr[z_start];
            calc_buff = buff_arr[z_start + 1];
        }

        for (int k = z_start; k <= z_end; ++k) {

            apply_stencil_7(matrix, k, calc_buff);

            if (k == z_start) {
                double *tmp = calc_buff;
                calc_buff = updt_buff;
                updt_buff = tmp;
            } else {
                double *tmp = calc_buff;
                calc_buff = matrix->arr[k - 1];
                matrix->arr[k - 1] = updt_buff;
                updt_buff = tmp;
            }
        }

        double *tmp = matrix->arr[z_end];
        matrix->arr[z_end] = updt_buff;
        updt_buff = tmp;
    }
}

static void apply_stencil_7(struct pc_matrix *matrix, int k, double *calc_buff) {

    double *pred = matrix->arr[k - 1];
    double *curr = matrix->arr[k];
    double *succ = matrix->arr[k + 1];

    int x = matrix->x;
    int y = matrix->y;
    int z = matrix->z;

    // Copy boundary values
    calc_buff[0 : x] = curr[0 : x]; // top
    calc_buff[(y - 1) * x : x] = curr[(y - 1) * x : x]; // bottom
    calc_buff[0 : y : x] = curr[0 : y : x]; // left
    calc_buff[(x - 1) : y : x] = curr[(x - 1) : y : x]; // right

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
