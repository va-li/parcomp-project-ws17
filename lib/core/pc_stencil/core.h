#ifndef PARCOMP_PROJECT_WS17_CORE_H
#define PARCOMP_PROJECT_WS17_CORE_H

struct pc_matrix {
    double **arr;
    int x;
    int y;
    int z;
};

#define PARSER_BUFFER_SIZE 256

// Given a pointer of an array `arr` it returns the pointer of the element in <x,y,z>
#define GET_PTR(arr, x, y, z)  (arr[z] + x * (y + 1))

int parcomp_parser_error;

/**
 * Sets the value of the array to a null pointer for safety
 *
 * @param matrix the matrix that holds the array
 */
void init_matrix(struct pc_matrix *matrix);

/**
 * If the array pointer is not set to null, it frees the memory and sets it tu null
 *
 * @param matrix the matrix that holds the array
 */
void destroy_matrix(struct pc_matrix *matrix);

/**
 * TODO
 * @param matrix
 */
void alloc_matrix(struct pc_matrix *matrix);

/**
 * Copy values from matrix SRC to matrix DEST. Both matrices are expected to be allocated before passing.
 * @param src matrix to copy from
 * @param dest matrix to copy to
 * @return 0 if the values were copied, -1 if the dimensions of SRC and DEST matrix did not match
 */
int copy_matrix(struct pc_matrix *src, struct pc_matrix *dest);

/**
 * Check two matrices if they have equal values
 * @param a
 * @param b
 * @return 1 if they are equal, 0 otherwise
 */
int equal_matrix(struct pc_matrix *a, struct pc_matrix *b);

/**
 * This parses the file with the filename and returns the parsed array
 *
 * IF THERE'S AN ERROR DURING PARSING, `parcomp_parser_error` will be set to the corresponding values
 *      1 - can't open file
 *      2 - file is empty
 *      4 - format is incorrect
 *
 * @param filename - the name of the file where the values are
 * @return the pointer to the array with parsed values
 */
struct pc_matrix parse(char* filename);

#endif //PARCOMP_PROJECT_WS17_CORE_H