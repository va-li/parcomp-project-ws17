#include "sequential.h"
#include "main/c/core/core.h"

#define ELEMENT(ARR, X, Y) ((ARR)[(X)*(Y) + (X)])

void stencil_plane_7(struct pc_matrix *matrix, double *buffer_plane, int z) {
    double* prev = matrix->arr[z - 1];
    double* curr = matrix->arr[z];
    double* futu = matrix->arr[z + 1];

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
            ELEMENT(buffer_plane, i, j) = tmp;
        }
    }
}

void stencil_plane_27(struct pc_matrix *matrix, double *buffer_plane, int z) {
    double* prev = matrix->arr[z - 1];
    double* curr = matrix->arr[z];
    double* futu = matrix->arr[z + 1];

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
            ELEMENT(buffer_plane, i, j) = tmp;
        }
    }
}

void run_stencil(struct pc_matrix *matrix) {
    // MAKE SURE YOU START FROM 1 and end to N-1
}
