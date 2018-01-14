#include <stdio.h>
#include <mpi.h>

#include "pc_stencil/mpi_stencil.h"

int run = 1;

int main(int argc, char **argv) {
    int rank, size;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (argc != 2) {
        if (rank == 0) {
            fprintf(stderr, "This executable can only be run with the name of input file as argument.\n");
        }
        run = 0;
    }

    struct pc_matrix matrix;

    if (run != 0) {
        matrix = parse_matrix_mpi(rank, size, argv[1]);
    }

    if (matrix.x == -1){
        if (rank == 0) {
            fprintf(stderr, "Matrix could not be parsed.\n");
        }
        run = 0;
    }

    // TODO : Add a barrier and start counting the time here!

    if (run != 0) {
        run_stencil_mpi(rank, size, &matrix);
    }

    // TODO : Add a barrier and stop counting. Do the calculation of the time and print it out

    if (run != 0) {
        MPI_Barrier(MPI_COMM_WORLD);
        print_matrix_mpi(rank, size, &matrix);
    }

    if (run != 0) {
        destroy_matrix(&matrix);
    }

    MPI_Finalize();
    return 0;
}