#include <stdio.h>
#include <mpi.h>

#include "pc_stencil/mpi_stencil.h"
#include "pc_stencil/utils.h"

int run = 1;

void print_benchmark(double seconds, int xyz, int cores, bool stencil7, bool human) {
    long sec = (long) seconds;
    long micro = (long) ((seconds - sec) * 1000000);
    micro += sec * 1000000;

    xyz -= 2;

    if (human) print_benchmark_human(micro, xyz, cores, stencil7, "MPI");
    else print_benchmark_csv(micro, xyz, cores, stencil7, "MPI");
}

int main(int argc, char **argv) {
    int rank, size;

    bool stencil7 = true, human = true, print_out = false;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (argc < 2 || argc > 3) {
        if (rank == 0) {
            print_common_usage();
        }
        run = 0;
    }

    struct pc_matrix matrix;

    if (run != 0) {
        matrix = parse_matrix_mpi(rank, size, argv[1]);

        if (argc == 3)
            switch (argv[2][0]) {
                case '1':
                    stencil7 = true;
                    human = false;
                    break;
                case '2':
                    stencil7 = false;
                    human = false;
                    break;
                case '3':
                    stencil7 = true;
                    human = true;
                    break;
                case '4':
                    stencil7 = false;
                    human = true;
                    break;
                default:
                    if (rank == 0) {
                        print_common_usage();
                    }
                    run = 0;
            }
        if (argv[2][1] == '+') {
            print_out = true;
        }

    }

    if (matrix.x == -1) {
        if (rank == 0) {
            fprintf(stderr, "Matrix could not be parsed.\n");
        }
        run = 0;
    }

    double start = 0, end = 0;
    if (run != 0) {
        MPI_Barrier(MPI_COMM_WORLD);
        start = MPI_Wtime();

        if (stencil7) run_mpi_stencil7(rank, size, &matrix);
        else run_mpi_stencil27(rank, size, &matrix);

        MPI_Barrier(MPI_COMM_WORLD);
        end = MPI_Wtime();
    }

    if (run != 0) {
        if (print_out) {
            MPI_Barrier(MPI_COMM_WORLD);
            print_matrix_mpi(rank, size, &matrix);
        } else if (rank == 0) {
            print_benchmark(end - start, matrix.x, size, stencil7, human);
        }
    }

    if (run != 0) {
        destroy_matrix(&matrix);
    }

    MPI_Finalize();
    return 0;
}