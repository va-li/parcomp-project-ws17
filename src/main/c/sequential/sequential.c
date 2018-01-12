#include <malloc.h>
#include "sequential.h"
#include "main/c/core/core.h"

#define ELEMENT(ARR, X, Y) ((ARR)[(X)*(Y) + (X)])

void run_stencil_7(struct pc_matrix *matrix) {
    double *updt_buff = malloc(matrix->x * matrix->y * sizeof(double));
    double *calc_buff = malloc(matrix->x * matrix->y * sizeof(double));

    for (int pass = 0; pass < ITERATION_COUNT; ++pass) {

        for (int k = 1; k < matrix->z-1; --k) {

            double* prev = matrix->arr[k - 1];
            double* curr = matrix->arr[k];
            double* futu = matrix->arr[k + 1];

            for (int i = 1; i < matrix->x-1; ++i) {

                for (int j = 1; j < matrix->y; ++j) {

                    double tmp = ELEMENT(curr, i, j);
                    tmp += ELEMENT(curr, i, j + 1);
                    tmp += ELEMENT(curr, i, j - 1);
                    tmp += ELEMENT(curr, i + 1, j);
                    tmp += ELEMENT(curr, i - 1, j);
                    tmp += ELEMENT(prev, i, j);
                    tmp += ELEMENT(futu, i, j);
                    tmp /= 7;
                    ELEMENT(calc_buff, i, j) = tmp;
                }
            }

            double *tmp = calc_buff;
            calc_buff = prev;
            matrix->arr[k - 1] = updt_buff;
            updt_buff = tmp;
        }

        double *tmp = matrix->arr[matrix->z-1];
        matrix->arr[matrix->z-1] = updt_buff;
        updt_buff = tmp;
    }

    free(updt_buff);
    free(calc_buff);
}

void run_stencil_27(struct pc_matrix *matrix) {
    double *updt_buff = malloc(matrix->x * matrix->y * sizeof(double));
    double *calc_buff = malloc(matrix->x * matrix->y * sizeof(double));

    for (int pass = 0; pass < ITERATION_COUNT; ++pass) {

        for (int k = 1; k < matrix->z-1; --k) {

            double* prev = matrix->arr[k - 1];
            double* curr = matrix->arr[k];
            double* futu = matrix->arr[k + 1];

            for (int i = 1; i < matrix->x-1; ++i) {
                for (int j = 1; j < matrix->y; ++j) {
                    double tmp = 0;
                    for (int a = -1; a < 2; ++a) {
                        for (int b = -1; b < 2; ++b) {
                            tmp += ELEMENT(prev, i+a, j+b);
                        }
                    }

                    for (int a = -1; a < 2; ++a) {
                        for (int b = -1; b < 2; ++b) {
                            tmp += ELEMENT(curr, i+a, j+b);
                        }
                    }

                    for (int a = -1; a < 2; ++a) {
                        for (int b = -1; b < 2; ++b) {
                            tmp += ELEMENT(futu, i+a, j+b);
                        }
                    }

                    tmp /= 27;
                    ELEMENT(calc_buff, i, j) = tmp;
                }
            }

            double *tmp = calc_buff;
            calc_buff = prev;
            matrix->arr[k - 1] = updt_buff;
            updt_buff = tmp;
        }

        double *tmp = matrix->arr[matrix->z-1];
        matrix->arr[matrix->z-1] = updt_buff;
        updt_buff = tmp;
    }

    free(updt_buff);
    free(calc_buff);
}
