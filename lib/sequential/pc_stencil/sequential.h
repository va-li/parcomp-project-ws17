#ifndef PARCOMP_PROJECT_WS17_SEQUENTIAL_H
#define PARCOMP_PROJECT_WS17_SEQUENTIAL_H

#include "pc_stencil/core.h"

/**
 *
 * @param org_matrix
 * @param tmp_matrix
 */
void run_naive_stencil_7(struct pc_matrix *org_matrix, struct pc_matrix *tmp_matrix);

/**
 *
 * @param org_matrix
 * @param tmp_matrix
 */
void run_naive_stencil_27(struct pc_matrix *org_matrix, struct pc_matrix *tmp_matrix);

/**
 * Applies the 7-stencil to <matrix> ITERATOR_COUNT times
 * Thi
 * @param matrix
 */
void run_stencil_7(struct pc_matrix *matrix);

/**
 * Applies the 27-stencil to <matrix> ITERATOR_COUNT times
 * @param matrix
 */
void run_stencil_27(struct pc_matrix *matrix);

#endif //PARCOMP_PROJECT_WS17_SEQUENTIAL_H
