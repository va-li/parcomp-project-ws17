#include <malloc.h>
#include <cilk/cilk.h>
#include "cilk_stencil.h"

static void one_plane_pass_7_stencil(struct pc_matrix *matrix, double *pred, double *curr, double *succ, double *buff);

void run_cilk_stencil_7(struct pc_matrix *matrix) {
    double *updt_buff = malloc(matrix->x * matrix->y * sizeof(double));
    double *calc_buff = malloc(matrix->x * matrix->y * sizeof(double));

    for (int pass = 0; pass < ITERATION_COUNT; ++pass) {

        for (int k = 1; k < matrix->z-1; ++k) {

            double* prev = matrix->arr[k - 1];
            double* curr = matrix->arr[k];
            double* succ = matrix->arr[k + 1];

            one_plane_pass_7_stencil(matrix, prev, curr, succ, calc_buff);

            if (k == 1) {
                double *tmp = calc_buff;
                calc_buff = updt_buff;
                updt_buff = tmp;
            } else {
                double *tmp = calc_buff;
                calc_buff = prev;
                matrix->arr[k - 1] = updt_buff;
                updt_buff = tmp;
            }
        }

        double *tmp = matrix->arr[matrix->z - 2];
        matrix->arr[matrix->z - 2] = updt_buff;
        updt_buff = tmp;
    }

    free(updt_buff);
    free(calc_buff);
}

static void one_plane_pass_7_stencil(struct pc_matrix *matrix, double *pred, double *curr, double *succ, double *buff) {
    int x = matrix->x;
    int y = matrix->y;
    int z = matrix->z;


    // Copy boundary values
    buff[0 : x] = curr[0 : x]; // top
    /*
    buff[(y-1)*x : x] = curr[(y-1)*x : x]; // bottom
    buff[0 : 1 : y] = curr[0 : 1 : y]; // left
    buff[(x-2) : 1 : y] = curr[(x-2) : 1 : y]; // right
*/

    for (int i = 0; i < matrix->x; ++i) {
        //ELEMENT(buff, x, i, 0) = ELEMENT(curr, x, i, 0);
        ELEMENT(buff, x, i, matrix->y-1) = ELEMENT(curr, x, i, matrix->y-1);
    }

    for (int j = 0; j < matrix->y; ++j) {
        ELEMENT(buff, x, 0, j) = ELEMENT(curr, x, 0, j);
        ELEMENT(buff, x, matrix->x-1, j) = ELEMENT(curr, x, matrix->x-1, j);
    }

    // Horizontally calculate lines (possibly in parallel
    /*cilk_for (int j = 1; j < y - 1; ++j) {
        int n = x-2;
        int m = y-2;
        // smash top, center and bottom line together
        buff[j*x : n] = curr[(j-1)*x : n] + curr[(j+1)*x : n];
        //smash previous and next line together
        buff[j*x : n] = buff[j*x : n] + pred[j*x : n] + succ[j*x : n];

        for (int i = 1; i < x - 1; ++i) {
            buff[i*j + i] = buff[i*j + i] + curr[i*j + i-1] + curr[i*j + i+1];
        }

        buff[1 : n] = buff[1 : n] / 7.0;
    }*/

    for (int i = 1; i < matrix->x - 1; ++i) {

        for (int j = 1; j < matrix->y - 1; ++j) {

            double tmp = ELEMENT(curr, x, i, j);
            tmp += ELEMENT(curr, x, i, j + 1);
            tmp += ELEMENT(curr, x, i, j - 1);
            tmp += ELEMENT(curr, x, i + 1, j);
            tmp += ELEMENT(curr, x, i - 1, j);
            tmp += ELEMENT(pred, x, i, j);
            tmp += ELEMENT(succ, x, i, j);
            tmp /= 7;
            ELEMENT(buff, x, i, j) = tmp;
        }
    }
}
