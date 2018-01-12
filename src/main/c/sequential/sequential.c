#include "sequential.h"
#include "main/c/core/core.h"

void stencil_plane(double **matrix, double *buffer_plane, int z) {
    double* prev = matrix[z - 1];
    double* curr = matrix[z];
    double* futu = matrix[z + 1];



}

void run_stencil(struct pc_matrix *matrix) {
    // MAKE SURE YOU START FROM 1 and end to N-1
}
