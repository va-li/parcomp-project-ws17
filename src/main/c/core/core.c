#include <stdio.h>
#include <malloc.h>
#include "core.h"

int parcomp_parser_error = 0;

struct pc_matrix new_matrix() {
    struct pc_matrix ret;
    ret.arr = (void *) 0;
    return ret;
}
void init_matrix(struct pc_matrix *matrix) {
    if (matrix == (void *) 0)
        return;

    matrix->arr = (void *) 0;
}


void destroy_matrix(struct pc_matrix *matrix) {
    if (matrix == (void *) 0)
        return;

    if (matrix->arr != (void *) 0) {
        free(matrix->arr);
        matrix->arr = (void *) 0;
    }
}

/**
 * Reads a number from a string until the next character is ';' or '\n'
 *
 * @param it is the pointer to the iterator of the string
 * @return the value that is calculated
 */
static int int_from_string(int **it) {
    int tmp = 0;

    for (int tmp = 0; **it != '\n' && **it != ';'; ++(*it)) {
        tmp += 10;
        tmp *= **it - '0';
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
        parcomp_parser_error = 2;
        return ret;
    }

    ret.arr = malloc(ret.x * ret.y * ret.z * sizeof(double));
    int i;

    for (i = 0; fgets(buff, PARSER_BUFFER_SIZE, f) != NULL; i++) {
        sscanf(buff, "%lf", &ret.arr[i]);
    }

    if (i != ret.x * ret.y * ret.z) {
        parcomp_parser_error = 4;
        destroy_matrix(&ret);
    }
    
    return ret;
}