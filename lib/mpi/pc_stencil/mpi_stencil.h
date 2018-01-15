#ifndef PARCOMP_PROJECT_WS17_MPI_STENCIL_H
#define PARCOMP_PROJECT_WS17_MPI_STENCIL_H

#include "pc_stencil/core.h"

struct pc_matrix parse_matrix_mpi(int mpi_rank, int mpi_size, char *filename);

int run_mpi_stencil7(int mpi_rank, int mpi_size, struct pc_matrix *matrix);

int run_mpi_stencil27(int mpi_rank, int mpi_size, struct pc_matrix *matrix);

int print_matrix_mpi(int mpi_rank, int mpi_size, struct pc_matrix *matrix);
#endif //PARCOMP_PROJECT_WS17_MPI_STENCIL_H
