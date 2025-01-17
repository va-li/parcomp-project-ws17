#ifndef PARCOMP_PROJECT_WS17_CORE_H
#define PARCOMP_PROJECT_WS17_CORE_H

#include <stdio.h>
#include <stdbool.h>

struct pc_matrix {
    double **arr;
    int x;
    int y;
    int z;
};

/**
 * Number of times a stencil is applied to the matrix
 */
#define ITERATION_COUNT (100)

int pc_openmp_sugessted_threads;

int pc_openmp_used_threads;

#define ELEMENT(ARR, LINE_LENGTH, X, Y) ((ARR)[(LINE_LENGTH)*(Y) + (X)])

#define PARSER_BUFFER_SIZE (256)

#define CMP_THRESHOLD (0.01)

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
 * Check two matrices if the difference between each value in a and b is smaller than the threshold
 * @param a
 * @param b
 * @param threshold
 * @return 1 if they are equal, 0 otherwise
 */
int equal_matrix(struct pc_matrix *a, struct pc_matrix *b, double threshold);

/**
 * Prints out the whole matrix to FP
 * @param fp
 * @param matrix
 * @param include_boundary_vals
 */
void print_matrix(FILE *fp, struct pc_matrix *matrix, bool include_boundary_vals);

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

/**
 * Reads a number from a string until the next character is ';' or '\n'
 *
 * @param it is the pointer to the iterator of the string
 * @return the value that is calculated
 */
int int_from_string(char **it);

/**
 * Gets X;Y;Z from a string and saves them to the given pc_matrix
 *
 * @param matrix the struct where we save the values for x y and z
 * @param string the string with the given values
 * @return 0 if successful, 1 if there's an error
 */
int xyz_from_string(struct pc_matrix *matrix, char *string);

#endif //PARCOMP_PROJECT_WS17_CORE_H
