#include <malloc.h>
#include <cilk/cilk.h>
#include "cilk_stencil.h"

void run_cilk_stencil_7(struct pc_matrix *matrix) {
    double *updt_buff = malloc(matrix->x * matrix->y * sizeof(double));
    double *calc_buff = malloc(matrix->x * matrix->y * sizeof(double));

    for (int pass = 0; pass < ITERATION_COUNT; ++pass) {

        for (int k = 1; k < matrix->z-1; ++k) {

            double* pred = matrix->arr[k - 1];
            double* curr = matrix->arr[k];
            double* succ = matrix->arr[k + 1];

            int x = matrix->x;
            int y = matrix->y;
            int z = matrix->z;

            // Copy boundary values
            calc_buff[0 : x] = curr[0 : x]; // top
            calc_buff[(y-1)*x : x] = curr[(y-1)*x : x]; // bottom
            calc_buff[0 : y : x] = curr[0 : y : x]; // left
            calc_buff[(x-1) : y : x] = curr[(x-1) : y : x]; // right

            cilk_for (int j = 1; j < y - 1; ++j) {
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

            /*
            // Does not produce correct output
            for (int j = 1; j < y - 1; j++) {
                int from = j * x + 1;
                int len = x-2;
                int top_from = (j-1) * x + 1;
                int bot_from = (j+1) * x + 1;
                int r_from = from + 1;
                int l_from = from - 1;
                buff[from:len] = (pred[from:len] + curr[from:len] + succ[from:len] + curr[top_from:len] + curr[bot_from:len] + curr[r_from:len] + curr[l_from:len]);
                buff[from:len] = buff[from:len] / 7;
            }*/

            if (k == 1) {
                double *tmp = calc_buff;
                calc_buff = updt_buff;
                updt_buff = tmp;
            } else {
                double *tmp = calc_buff;
                calc_buff = pred;
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
