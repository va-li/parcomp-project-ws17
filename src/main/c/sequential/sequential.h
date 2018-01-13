#ifndef PARCOMP_PROJECT_WS17_SEQUENTIAL_H
#define PARCOMP_PROJECT_WS17_SEQUENTIAL_H

#include "main/c/core/core.h"

/**
 * Number of times a stencil is applied to the matrix
 */
#define ITERATION_COUNT (100)

/**
 *
 * @param src_matrix
 * @param dest_matrix
 */
void run_naive_stencil_7(struct pc_matrix *src_matrix, struct pc_matrix *dest_matrix);

/**
 * Applies the 7-stencil to <matrix> ITERATOR_COUNT times
 * @param matrix
 */
void run_stencil_7(struct pc_matrix *matrix);

/**
 * Applies the 27-stencil to <matrix> ITERATOR_COUNT times
 * @param matrix
 */
void run_stencil_27(struct pc_matrix *matrix);

#endif //PARCOMP_PROJECT_WS17_SEQUENTIAL_H
