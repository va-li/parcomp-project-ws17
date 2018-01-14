#include "openmp.h"

#include <malloc.h>
#include <omp.h>

/**
 * INLINE calculates the start and end positions of the "chunks" for each thread based on their id and the number
 * of all threads
 * @param start pointer where to save the value
 * @param end pointer where to save the value
 * @param matrix information about the size of Z
 * @param num_of_threads number of threads that are running
 * @param id the id of the current thread
 */
inline static void calculate_values(int *start, int *end, struct pc_matrix *matrix, int num_of_threads, int id) {
    int m_val = matrix->z - 2;
    int chunk_size = m_val / num_of_threads;
    *start = id * chunk_size + 1;
    if (id == num_of_threads - 1 && m_val % num_of_threads != 0) {
        *end = m_val + 1;
    } else {
        *end = (id + 1) * chunk_size + 1;
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

        int start;
        int end;
        calculate_values(&start, &end, matrix, omp_get_num_threads(), id);

        for (int pass = 0; pass < ITERATION_COUNT; ++pass) {

            for (int k = start; k < end; ++k) {

                double *prev = matrix->arr[k - 1];
                double *curr = matrix->arr[k];
                double *futu = matrix->arr[k + 1];

                // Copy boundary values
                for (int i = 0; i < matrix->x; ++i) {
                    ELEMENT(calc_buff_arr[id], i, 0) = ELEMENT(curr, i, 0);
                    ELEMENT(calc_buff_arr[id], i, matrix->y - 1) = ELEMENT(curr, i, matrix->y - 1);
                }

                for (int j = 0; j < matrix->y; ++j) {
                    ELEMENT(calc_buff_arr[id], 0, j) = ELEMENT(curr, 0, j);
                    ELEMENT(calc_buff_arr[id], matrix->x - 1, j) = ELEMENT(curr, matrix->x - 1, j);
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
                } else if (k == start + 1) {
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

            double *tmp = matrix->arr[end - 1];
            matrix->arr[end - 1] = updt_buff_arr[id];
            updt_buff_arr[id] = tmp;

            tmp = matrix->arr[start];
            matrix->arr[start] = first_buff_arr[id];
            first_buff_arr[id] = tmp;

#pragma omp barrier
        }
    }

    for (int i = 0; i < NUM_THREADS; ++i) {
        free(first_buff_arr[i]);
        free(updt_buff_arr[i]);
        free(calc_buff_arr[i]);
    }

}

void run_openmp_stencil_27(struct pc_matrix *matrix) {
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

        int start;
        int end;
        calculate_values(&start, &end, matrix, omp_get_num_threads(), id);

        for (int pass = 0; pass < ITERATION_COUNT; ++pass) {

            for (int k = start; k < end; ++k) {

                double *prev = matrix->arr[k - 1];
                double *curr = matrix->arr[k];
                double *futu = matrix->arr[k + 1];

                // Copy boundary values
                for (int i = 0; i < matrix->x; ++i) {
                    ELEMENT(calc_buff_arr[id], i, 0) = ELEMENT(curr, i, 0);
                    ELEMENT(calc_buff_arr[id], i, matrix->y - 1) = ELEMENT(curr, i, matrix->y - 1);
                }

                for (int j = 0; j < matrix->y; ++j) {
                    ELEMENT(calc_buff_arr[id], 0, j) = ELEMENT(curr, 0, j);
                    ELEMENT(calc_buff_arr[id], matrix->x - 1, j) = ELEMENT(curr, matrix->x - 1, j);
                }

                for (int i = 1; i < matrix->x - 1; ++i) {

                    for (int j = 1; j < matrix->y - 1; ++j) {

                        double tmp = 0;
                        for (int a = -1; a < 2; ++a) {
                            for (int b = -1; b < 2; ++b) {
                                tmp += ELEMENT(prev, i + a, j + b);
                                tmp += ELEMENT(curr, i + a, j + b);
                                tmp += ELEMENT(futu, i + a, j + b);
                            }
                        }

                        tmp /= 27;

                        ELEMENT(calc_buff_arr[id], i, j) = tmp;
                    }
                }

                if (k == start) {
                    double *tmp = calc_buff_arr[id];
                    calc_buff_arr[id] = first_buff_arr[id];
                    first_buff_arr[id] = tmp;
                } else if (k == start + 1) {
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

            double *tmp = matrix->arr[end - 1];
            matrix->arr[end - 1] = updt_buff_arr[id];
            updt_buff_arr[id] = tmp;

            tmp = matrix->arr[start];
            matrix->arr[start] = first_buff_arr[id];
            first_buff_arr[id] = tmp;

#pragma omp barrier
        }
    }

    for (int i = 0; i < NUM_THREADS; ++i) {
        free(first_buff_arr[i]);
        free(updt_buff_arr[i]);
        free(calc_buff_arr[i]);
    }

}
