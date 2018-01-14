#include <stdio.h>
#include <malloc.h>
#include <stdlib.h>
#include "core.h"

int parcomp_parser_error = 0;

void init_matrix(struct pc_matrix *matrix) {
    if (matrix == (void *) 0)
        return;

    matrix->arr = (void *) 0;
}

void alloc_matrix(struct pc_matrix *matrix) {
    matrix->arr = malloc(matrix->z * sizeof(double *));

    for (int i = 0; i < matrix->z; ++i) {
        matrix->arr[i] = malloc(matrix->x * matrix->y * sizeof(double));
    }
}

void destroy_matrix(struct pc_matrix *matrix) {
    if (matrix == (void *) 0)
        return;

    if (matrix->arr == (void *) 0)
        return;

    for (int i = 0; i < matrix->z; ++i) {
        if (matrix->arr[i] == (void *) 0)
            continue;
        free(matrix->arr[i]);
    }

    free(matrix->arr);
    matrix->arr = (void *) 0;
}

int copy_matrix(struct pc_matrix *src, struct pc_matrix *dest) {
    if ((src->x != dest->x)
        || (src->y != dest->y)
        ||(src->z != dest->z)) {
        return -1;
    }

    for (int k = 0; k < src->z; ++k) {
        for (int ij = 0; ij < src->x * src->y; ++ij) {
            dest->arr[k][ij] = src->arr[k][ij];
        }
    }

    return 0;
}

int equal_matrix(struct pc_matrix *a, struct pc_matrix *b) {
    if ((a->x != b->x)
        || (a->y != b->y)
        ||(a->z != b->z)) {
        return 0;
    }

    int ret = 1;

    for (int k = 0; k < a->z; ++k) {
        for (int ij = 0; ij < a->x*a->y; ++ij) {
            if (a->arr[k][ij] != b->arr[k][ij]) ret = 0;
        }
    }

    return ret;
}

void print_matrix(struct pc_matrix *matrix, bool include_boundary_vals) {
    if (include_boundary_vals) {
        for (int k = 0; k < matrix->z; k++) {
            for (int ij = 0; ij < matrix->x * matrix->y; ij++) {
                printf("%f\n", matrix->arr[k][ij]);
            }
        }
    } else {
        for (int k = 1; k < matrix->z - 1; k++) {
            for (int j = 1; j < matrix->y - 1; ++j) {
                for (int i = 1; i < matrix->x - 1; ++i) {
                    printf("%f\n", matrix->arr[k][i * j + i]);
                }
            }
        }
    }
}

/**
 * Reads a number from a string until the next character is ';' or '\n'
 *
 * @param it is the pointer to the iterator of the string
 * @return the value that is calculated
 */
int int_from_string(char **it) {
    int tmp = 0;

    while (**it != '\n' && **it != ';') {
        tmp *= 10;
        tmp += **it - '0';
        ++(*it);
    }

    return tmp;
}

/**
 * Gets X;Y;Z from a string and saves them to the given pc_matrix
 *
 * @param matrix the struct where we save the values for x y and z
 * @param string the string with the given values
 * @return 0 if successful, 1 if there's an error
 */
int xyz_from_string(struct pc_matrix *matrix, char *string) {
    if (matrix == (void *) 0 || string == (void *) 0)
        return 1;

    char *it = string;
    matrix->x = int_from_string(&it);

    if (matrix->x == 0 || *it == '\n')
        return 1;
    else
        ++it;

    matrix->y = int_from_string(&it);

    if (matrix->y == 0 || *it == '\n')
        return 1;
    else
        ++it;

    matrix->z = int_from_string(&it);

    if (matrix->z == 0)
        return 1;

    return 0;
}

struct pc_matrix parse(char* filename) {
    struct pc_matrix ret;
    init_matrix(&ret);

    FILE *f = fopen(filename, "r");

    if (f == NULL) {
        parcomp_parser_error = 1;
        return ret;
    }

    char buff[PARSER_BUFFER_SIZE];

    if (fgets(buff, PARSER_BUFFER_SIZE, f) == NULL) {
        parcomp_parser_error = 2;
        return ret;
    }

    if (xyz_from_string(&ret, buff)) {
        parcomp_parser_error = 4;
        return ret;
    }

    alloc_matrix(&ret);

    int i;
    char *ignore;

    int plane_size = ret.x * ret.y;

    for (i = 0; fgets(buff, PARSER_BUFFER_SIZE, f) != NULL; i++) {
        ret.arr[i / plane_size][i % plane_size] = strtod(buff, &ignore);
    }

    if (i != ret.x * ret.y * ret.z) {
        parcomp_parser_error = 4;
        destroy_matrix(&ret);
    }

    return ret;
}