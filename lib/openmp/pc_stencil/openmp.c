#include "openmp.h"

#include <stdio.h>
#include <omp.h>
#include <malloc.h>

void test() {
    int i = 25;
    printf("%d\n",i);

    #pragma omp parallel
    {
        int ID = omp_get_thread_num();
        printf("hello(%d) ", ID);
        printf("world(%d) \n", ID);
    }
}

void run_openmp_stencil_7(struct pc_matrix *matrix) {
    double *first_buff_arr[NUM_THREADS];
    double *updt_buff_arr[NUM_THREADS];
    double *calc_buff_arr[NUM_THREADS];

    for (int i = 0; i < NUM_THREADS; ++i) {
        first_buff_arr[i] = malloc(matrix->x * matrix->y * sizeof(double));
        updt_buff_arr[i] = malloc(matrix->x * matrix->y * sizeof(double));
        calc_buff_arr[i] = malloc(matrix->x * matrix->y * sizeof(double));
    }

    omp_set_num_threads(NUM_THREADS);

    #pragma omp parallel
    {
        int id = omp_get_thread_num();
        int start = id * ((matrix->z - 2) / NUM_THREADS) + 1;
        int end = start + (matrix->z - 2) / NUM_THREADS;

        for (int pass = 0; pass < ITERATION_COUNT; ++pass) {
            
            for (int k = start; k < end; ++k) {

                double* prev = matrix->arr[k - 1];
                double* curr = matrix->arr[k];
                double* futu = matrix->arr[k + 1];

                // Copy boundary values
                for (int i = 0; i < matrix->x; ++i) {
                    ELEMENT(calc_buff_arr[id], i, 0) = ELEMENT(curr, i, 0);
                    ELEMENT(calc_buff_arr[id], i, matrix->y-1) = ELEMENT(curr, i, matrix->y-1);
                }

                for (int j = 0; j < matrix->y; ++j) {
                    ELEMENT(calc_buff_arr[id], 0, j) = ELEMENT(curr, 0, j);
                    ELEMENT(calc_buff_arr[id], matrix->x-1, j) = ELEMENT(curr, matrix->x-1, j);
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
                        ELEMENT(calc_buff_arr[id], i, j) = tmp;
                    }
                }

                if (k == start) {
                    double *tmp = calc_buff_arr[id];
                    calc_buff_arr[id] = first_buff_arr[id];
                    first_buff_arr[id] = tmp;
                } else if (k == start+1) {
                    double *tmp = calc_buff_arr[id];
                    calc_buff_arr[id] = updt_buff_arr[id];
                    updt_buff_arr[id] = tmp;
                } else {
                    double *tmp = calc_buff_arr[id];
                    calc_buff_arr[id] = prev;
                    matrix->arr[k - 1] = updt_buff_arr[id];
                    updt_buff_arr[id] = tmp;
                }
            }

            #pragma omp barrier

            double *tmp = matrix->arr[end-1];
            matrix->arr[end-1] = updt_buff_arr[id];
            updt_buff_arr[id] = tmp;

            tmp = matrix->arr[start];
            matrix->arr[start] = first_buff_arr[id];
            first_buff_arr[id] = tmp;

            #pragma omp barrier
        }
        #pragma omp barrier
    }

    for (int i = 0; i < NUM_THREADS; ++i) {
        free(first_buff_arr[i]);
        free(updt_buff_arr[i]);
        free(calc_buff_arr[i]);
    }

}

void run_openmp_stencil_27(struct pc_matrix *matrix) {

}
