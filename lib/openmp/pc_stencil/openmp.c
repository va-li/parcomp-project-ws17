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
    if (num_of_threads > pc_openmp_used_threads) pc_openmp_used_threads = num_of_threads;
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
    omp_set_num_threads(pc_openmp_sugessted_threads);

#pragma omp parallel
    {
        double *first_buff = malloc(matrix->x * matrix->y * sizeof(double));
        double *updt_buff = malloc(matrix->x * matrix->y * sizeof(double));
        double *calc_buff = malloc(matrix->x * matrix->y * sizeof(double));

        int line_length = matrix->x;

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
                    ELEMENT(calc_buff, line_length, i, 0) = ELEMENT(curr, line_length, i, 0);
                    ELEMENT(calc_buff, line_length, i, matrix->y - 1) = ELEMENT(curr, line_length, i,
                                                                                matrix->y - 1);
                }

                for (int j = 0; j < matrix->y; ++j) {
                    ELEMENT(calc_buff, line_length, 0, j) = ELEMENT(curr, line_length, 0, j);
                    ELEMENT(calc_buff, line_length, matrix->x - 1, j) = ELEMENT(curr, line_length,
                                                                                matrix->x - 1, j);
                }

                for (int j = 1; j < matrix->y - 1; ++j) {
                    for (int i = 1; i < matrix->x - 1; ++i) {

                        double tmp = ELEMENT(curr, line_length, i, j);
                        tmp += ELEMENT(curr, line_length, i, j + 1);
                        tmp += ELEMENT(curr, line_length, i, j - 1);
                        tmp += ELEMENT(curr, line_length, i + 1, j);
                        tmp += ELEMENT(curr, line_length, i - 1, j);
                        tmp += ELEMENT(prev, line_length, i, j);
                        tmp += ELEMENT(futu, line_length, i, j);
                        tmp /= 7;
                        ELEMENT(calc_buff, line_length, i, j) = tmp;
                    }
                }

                if (k == start) {
                    double *tmp = calc_buff;
                    calc_buff = first_buff;
                    first_buff = tmp;
                } else if (k == start + 1) {
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

#pragma omp barrier

            double *tmp = matrix->arr[end - 1];
            matrix->arr[end - 1] = updt_buff;
            updt_buff = tmp;

            tmp = matrix->arr[start];
            matrix->arr[start] = first_buff;
            first_buff = tmp;

#pragma omp barrier
        }
        free(first_buff);
        free(updt_buff);
        free(calc_buff);
    }
}

void run_openmp_stencil_27(struct pc_matrix *matrix) {
    omp_set_num_threads(pc_openmp_sugessted_threads);

#pragma omp parallel
    {
        double *first_buff = malloc(matrix->x * matrix->y * sizeof(double));
        double *updt_buff = malloc(matrix->x * matrix->y * sizeof(double));
        double *calc_buff = malloc(matrix->x * matrix->y * sizeof(double));

        int line_length = matrix->x;

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
                    ELEMENT(calc_buff, line_length, i, 0) = ELEMENT(curr, line_length, i, 0);
                    ELEMENT(calc_buff, line_length, i, matrix->y - 1) = ELEMENT(curr, line_length, i,
                                                                                matrix->y - 1);
                }

                for (int j = 0; j < matrix->y; ++j) {
                    ELEMENT(calc_buff, line_length, 0, j) = ELEMENT(curr, line_length, 0, j);
                    ELEMENT(calc_buff, line_length, matrix->x - 1, j) = ELEMENT(curr, line_length,
                                                                                matrix->x - 1, j);
                }

                for (int j = 1; j < matrix->y - 1; ++j) {
                    for (int i = 1; i < matrix->x - 1; ++i) {

                        double tmp = 0;
                        for (int a = -1; a < 2; ++a) {
                            for (int b = -1; b < 2; ++b) {
                                tmp += ELEMENT(prev, line_length, i + a, j + b);
                                tmp += ELEMENT(curr, line_length, i + a, j + b);
                                tmp += ELEMENT(futu, line_length, i + a, j + b);
                            }
                        }

                        tmp /= 27;

                        ELEMENT(calc_buff, line_length, i, j) = tmp;
                    }
                }

                if (k == start) {
                    double *tmp = calc_buff;
                    calc_buff = first_buff;
                    first_buff = tmp;
                } else if (k == start + 1) {
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

#pragma omp barrier

            double *tmp = matrix->arr[end - 1];
            matrix->arr[end - 1] = updt_buff;
            updt_buff = tmp;

            tmp = matrix->arr[start];
            matrix->arr[start] = first_buff;
            first_buff = tmp;

#pragma omp barrier
        }
        free(first_buff);
        free(updt_buff);
        free(calc_buff);
    }

}
