#ifndef PARCOMP_PROJECT_WS17_CILK_STENCIL_H
#define PARCOMP_PROJECT_WS17_CILK_STENCIL_H

#include "pc_stencil/core.h"

/**
 * Applies the 7-stencil to <matrix> ITERATOR_COUNT times
 * @param matrix
 */
void run_cilk_stencil_7(struct pc_matrix *matrix);
void run_cilk_stencil_27(struct pc_matrix *matrix);

#endif //PARCOMP_PROJECT_WS17_CILK_STENCIL_H
