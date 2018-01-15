#include "mpi_stencil.h"

#include <mpi.h>
#include <stdlib.h>
#include <math.h>

inline static void calculate_load_values(int *start, int *end, int z, int mpi_rank, int mpi_size) {
    // Number of stencils that have to be calculated
    int m_val = z - 2;
    // Number of stencils per process
    int chunk_size = m_val / mpi_size;
    // it should actually start one plane earlier and end one later so that it can use them for calculation
    *start = mpi_rank * chunk_size;
    // it should end one position later for the same reason
    if (mpi_rank == mpi_size - 1 && m_val % mpi_size != 0) {
        *end = m_val + 2;
    } else {
        *end = (mpi_rank + 1) * chunk_size + 2;
    }
}

struct pc_matrix parse_matrix_mpi(int mpi_rank, int mpi_size, char *filename) {
    struct pc_matrix ret;
    init_matrix(&ret);

    FILE *f = fopen(filename, "r");

    if (f == NULL) {
        fprintf(stderr, "%d coulnd open file: %s\n", mpi_rank, filename);
        ret.x = -1;
        return ret;
    }
    char buff[PARSER_BUFFER_SIZE];

    if (fgets(buff, PARSER_BUFFER_SIZE, f) == NULL) {
        if (mpi_rank == 0)
            fprintf(stderr, "%d: %s is empty.\n", mpi_rank, filename);
        ret.x = -1;
        return ret;
    }

    if (xyz_from_string(&ret, buff)) {
        if (mpi_rank == 0)
            fprintf(stderr, "%d: %s contains the wrong layout for X;Y;Z\n", mpi_rank, filename);
        ret.x = -1;
        return ret;
    }

    int start, end;
    calculate_load_values(&start, &end, ret.z, mpi_rank, mpi_size);


    ret.z = end - start;

    alloc_matrix(&ret);

    int i;
    char *ignore;

    int plane_size = ret.x * ret.y;

    // TODO : Can improve this by going directly to the position on the file using `fseek`
    for (i = 0; fgets(buff, PARSER_BUFFER_SIZE, f) != NULL; i++) {
        int old_z = i / plane_size;
        if (old_z >= end) {
            break;
        }
        if (old_z >= start)
            ret.arr[old_z - start][i % plane_size] = strtod(buff, &ignore);
    }

    if (i - start * plane_size != ret.x * ret.y * ret.z) {
        fprintf(stderr, "%d: Number of lines in %s is too small\n", mpi_rank, filename);
        fprintf(stderr, "expected %d got %d \n", ret.x * ret.y * ret.z, i - start * plane_size);
        ret.x = -1;
        return ret;
    }

    return ret;
}

int run_mpi_stencil7(int mpi_rank, int mpi_size, struct pc_matrix *matrix) {
    double *updt_buff = malloc(matrix->x * matrix->y * sizeof(double));
    double *calc_buff = malloc(matrix->x * matrix->y * sizeof(double));
    int line_length = matrix->x;
    int plane_length = matrix->x * matrix->y;

    for (int pass = 0; pass < ITERATION_COUNT; ++pass) {

        for (int k = 1; k < matrix->z - 1; ++k) {

            double *prev = matrix->arr[k - 1];
            double *curr = matrix->arr[k];
            double *futu = matrix->arr[k + 1];

            // Copy boundary values
            for (int i = 0; i < matrix->x; ++i) {
                ELEMENT(calc_buff, line_length, i, 0) = ELEMENT(curr, line_length, i, 0);
                ELEMENT(calc_buff, line_length, i, matrix->y - 1) = ELEMENT(curr, line_length, i, matrix->y - 1);
            }

            for (int j = 0; j < matrix->y; ++j) {
                ELEMENT(calc_buff, line_length, 0, j) = ELEMENT(curr, line_length, 0, j);
                ELEMENT(calc_buff, line_length, matrix->x - 1, j) = ELEMENT(curr, line_length, matrix->x - 1, j);
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

#define LEFT_MSG (pass*10+1)
#define RIGHT_MSG (pass*10+2)

        MPI_Request tmp1;
        if (mpi_rank != 0) {
            // Send it to the left one
            MPI_Isend(matrix->arr[1], plane_length, MPI_DOUBLE, mpi_rank - 1, LEFT_MSG, MPI_COMM_WORLD, &tmp1);
        }

        MPI_Request tmp2;
        if (mpi_rank != mpi_size - 1) {
            // Send it to the left one
            MPI_Isend(matrix->arr[matrix->z - 2], plane_length, MPI_DOUBLE, mpi_rank + 1, RIGHT_MSG, MPI_COMM_WORLD,
                      &tmp2);
        }


        if (mpi_rank != 0) {
            // Receive from left
            MPI_Recv(matrix->arr[0], plane_length, MPI_DOUBLE, mpi_rank - 1, RIGHT_MSG, MPI_COMM_WORLD,
                     MPI_STATUS_IGNORE);
        }

        if (mpi_rank != mpi_size - 1) {
            // Receive from right
            MPI_Recv(matrix->arr[matrix->z - 1], plane_length, MPI_DOUBLE, mpi_rank + 1, LEFT_MSG, MPI_COMM_WORLD,
                     MPI_STATUS_IGNORE);
        }
        if (mpi_rank != 0) MPI_Wait(&tmp1, MPI_STATUS_IGNORE);
        if (mpi_rank != mpi_size - 1) MPI_Wait(&tmp2, MPI_STATUS_IGNORE);
    }

    free(updt_buff);
    free(calc_buff);
}

int run_mpi_stencil27(int mpi_rank, int mpi_size, struct pc_matrix *matrix) {
    double *updt_buff = malloc(matrix->x * matrix->y * sizeof(double));
    double *calc_buff = malloc(matrix->x * matrix->y * sizeof(double));
    int line_length = matrix->x;
    int plane_length = matrix->x * matrix->y;

    for (int pass = 0; pass < ITERATION_COUNT; ++pass) {

        for (int k = 1; k < matrix->z - 1; ++k) {

            double *prev = matrix->arr[k - 1];
            double *curr = matrix->arr[k];
            double *futu = matrix->arr[k + 1];

            // Copy boundary values
            for (int i = 0; i < matrix->x; ++i) {
                ELEMENT(calc_buff, line_length, i, 0) = ELEMENT(curr, line_length, i, 0);
                ELEMENT(calc_buff, line_length, i, matrix->y - 1) = ELEMENT(curr, line_length, i, matrix->y - 1);
            }

            for (int j = 0; j < matrix->y; ++j) {
                ELEMENT(calc_buff, line_length, 0, j) = ELEMENT(curr, line_length, 0, j);
                ELEMENT(calc_buff, line_length, matrix->x - 1, j) = ELEMENT(curr, line_length, matrix->x - 1, j);
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

#define LEFT_MSG (pass*10+1)
#define RIGHT_MSG (pass*10+2)

        MPI_Request tmp1;
        if (mpi_rank != 0) {
            // Send it to the left one
            MPI_Isend(matrix->arr[1], plane_length, MPI_DOUBLE, mpi_rank - 1, LEFT_MSG, MPI_COMM_WORLD, &tmp1);
        }

        MPI_Request tmp2;
        if (mpi_rank != mpi_size - 1) {
            // Send it to the left one
            MPI_Isend(matrix->arr[matrix->z - 2], plane_length, MPI_DOUBLE, mpi_rank + 1, RIGHT_MSG, MPI_COMM_WORLD,
                      &tmp2);
        }


        if (mpi_rank != 0) {
            // Receive from left
            MPI_Recv(matrix->arr[0], plane_length, MPI_DOUBLE, mpi_rank - 1, RIGHT_MSG, MPI_COMM_WORLD,
                     MPI_STATUS_IGNORE);
        }

        if (mpi_rank != mpi_size - 1) {
            // Receive from right
            MPI_Recv(matrix->arr[matrix->z - 1], plane_length, MPI_DOUBLE, mpi_rank + 1, LEFT_MSG, MPI_COMM_WORLD,
                     MPI_STATUS_IGNORE);
        }
        if (mpi_rank != 0) MPI_Wait(&tmp1, MPI_STATUS_IGNORE);
        if (mpi_rank != mpi_size - 1) MPI_Wait(&tmp2, MPI_STATUS_IGNORE);
    }

    free(updt_buff);
    free(calc_buff);
}

int print_matrix_mpi(int mpi_rank, int mpi_size, struct pc_matrix *matrix) {

    // TODO : Find a better way for locking the threads (waiting)
    // Wait if not zero
    int counter = 0;
    while (mpi_rank != counter) {
        MPI_Barrier(MPI_COMM_WORLD);
        counter++;
    }

    for (int z = 1; z < matrix->z - 1; ++z) {
        for (int y = 1; y < matrix->y - 1; ++y) {
            for (int x = 1; x < matrix->x - 1; ++x) {
                printf("%.02f\n", ELEMENT(matrix->arr[z], matrix->x, x, y));
            }
        }

    }


    // Let the next one continue if not last
    while (counter != mpi_size - 1) {
        MPI_Barrier(MPI_COMM_WORLD);
        counter++;
    }

}
