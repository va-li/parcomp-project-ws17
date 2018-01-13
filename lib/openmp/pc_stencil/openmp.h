#ifndef PARCOMP_PROJECT_WS17_OPENMP_H
#define PARCOMP_PROJECT_WS17_OPENMP_H

#include "pc_stencil/core.h"

/**
 * Applies the 7-stencil to <matrix> ITERATOR_COUNT times using OpenMP
 * @param matrix
 */
void run_openmp_stencil_7(struct pc_matrix *matrix);

/**
 * Applies the 27-stencil to <matrix> ITERATOR_COUNT times using OpenMP
 * @param matrix
 */
void run_openmp_stencil_27(struct pc_matrix *matrix);

#endif //PARCOMP_PROJECT_WS17_OPENMP_H
