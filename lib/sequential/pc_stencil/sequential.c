#include <malloc.h>
#include "sequential.h"

#define ELEMENT(ARR, X, Y) ((ARR)[(X)*(Y) + (X)])

void run_naive_stencil_7(struct pc_matrix *src_matrix, struct pc_matrix *dest_matrix) {
    // Necessary to copy the boundary values into DEST_MATRIX
    copy_matrix(src_matrix, dest_matrix);

    for (int pass = 0; pass < ITERATION_COUNT; ++pass) {
        for (int i = 1; i < src_matrix->x - 1; ++i) {
            for (int j = 1; j < src_matrix->y - 1; ++j) {
                for (int k = 1; k < src_matrix->z - 1; ++k) {
                    double tmp = ELEMENT(src_matrix->arr[k], i, j);
                    tmp += ELEMENT(src_matrix->arr[k], i, j + 1);
                    tmp += ELEMENT(src_matrix->arr[k], i, j - 1);
                    tmp += ELEMENT(src_matrix->arr[k], i + 1, j);
                    tmp += ELEMENT(src_matrix->arr[k], i - 1, j);
                    tmp += ELEMENT(src_matrix->arr[k - 1], i, j);
                    tmp += ELEMENT(src_matrix->arr[k + 1], i, j);
                    tmp /= 7;
                    ELEMENT(dest_matrix->arr[k], i, j) = tmp;
                }
            }
        }
        double **tmp = src_matrix->arr;
        src_matrix->arr = dest_matrix->arr;
        dest_matrix->arr = tmp;
    }

    double **tmp = src_matrix->arr;
    src_matrix->arr = dest_matrix->arr;
    dest_matrix->arr = tmp;
}

void run_stencil_7(struct pc_matrix *matrix) {
    double *updt_buff = malloc(matrix->x * matrix->y * sizeof(double));
    double *calc_buff = malloc(matrix->x * matrix->y * sizeof(double));

    for (int pass = 0; pass < ITERATION_COUNT; ++pass) {

        for (int k = 1; k < matrix->z-1; ++k) {

            double* prev = matrix->arr[k - 1];
            double* curr = matrix->arr[k];
            double* futu = matrix->arr[k + 1];

            // Copy boundary values
            for (int i = 0; i < matrix->x; ++i) {
                ELEMENT(calc_buff, i, 0) = ELEMENT(curr, i, 0);
                ELEMENT(calc_buff, i, matrix->y-1) = ELEMENT(curr, i, matrix->y-1);
                //ELEMENT(calc_buff, (matrix->x) - 1 - i, 0) = ELEMENT(curr, (matrix->x) - 1 - i, 0);
            }

            for (int j = 0; j < matrix->y; ++j) {
                ELEMENT(calc_buff, 0, j) = ELEMENT(curr, 0, j);
                ELEMENT(calc_buff, matrix->x-1, j) = ELEMENT(curr, matrix->x-1, j);
                //ELEMENT(calc_buff, 0, (matrix->y) - 1 - j) = ELEMENT(curr, 0, (matrix->y) - 1 - j);
            }

            for (int i = 1; i < matrix->x - 1; ++i) {

                for (int j = 1; j < matrix->y - 1; ++j) {

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

/*void run_stencil_27(struct pc_matrix *matrix) {
    double *updt_buff = malloc(matrix->x * matrix->y * sizeof(double));
    double *calc_buff = malloc(matrix->x * matrix->y * sizeof(double));

    for (int pass = 0; pass < ITERATION_COUNT; ++pass) {

        for (int k = 1; k < matrix->z-1; ++k) {

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
}*/
