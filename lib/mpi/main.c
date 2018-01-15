#include <stdio.h>
#include <mpi.h>

#include "pc_stencil/mpi_stencil.h"

int run = 1;

void print_time_human(double seconds) {
    int sec = (int) seconds;
    int milli = (int) ((seconds - sec) * 1000);
    int min = sec / 60;
    int hr = min / 60;
    sec %= 60;
    printf("%dh %dm %ds %dmilli\n", hr, min, sec, milli);
}

void print_time_csv(double seconds, int xyz, int cores, bool stencil7) {
    long sec = (long) seconds;
    long micro = (long) ((seconds - sec) * 1000000);
    micro += sec * 1000000;
    xyz -= 2:
    // XxYxZ,stencil_type,MPI,cores,microseconds
    printf("%dx%dx%d,%d,MPI,%d,%ld\n",xyz, xyz, xyz, (stencil7?7:27), cores, micro);
}

int main(int argc, char **argv) {
    int rank, size;

    bool stencil7 = true, human = true;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (argc < 2 || argc > 3) {
        if (rank == 0) {
            fprintf(stderr, "usage: ./pc_stencil_mpi filename [mode]\n");
            fprintf(stderr, "\tonly the first character is checked for the mode\n");
            fprintf(stderr, "Modes:\n");
            fprintf(stderr, "\t\t 1 - Stencil7 csv\n");
            fprintf(stderr, "\t\t 2 - Stencil27 csv\n");
            fprintf(stderr, "\t\t 3 - Stencil7 human readable (default if empty)\n");
            fprintf(stderr, "\t\t 4 - Stencil27 human readable\n");
        }
        run = 0;
    }

    struct pc_matrix matrix;

    if (run != 0) {
        matrix = parse_matrix_mpi(rank, size, argv[1]);

        if (argc == 3) switch (argv[2][0]) {
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
                        fprintf(stderr, "usage: ./pc_stencil_mpi filename [mode]\n");
                        fprintf(stderr, "\tonly the first character is checked for the mode\n");
                        fprintf(stderr, "Modes:\n");
                        fprintf(stderr, "\t\t 1 - Stencil7 csv\n");
                        fprintf(stderr, "\t\t 2 - Stencil27 csv\n");
                        fprintf(stderr, "\t\t 3 - Stencil7 human readable (default if empty)\n");
                        fprintf(stderr, "\t\t 4 - Stencil27 human readable\n");
                    }
                    run = 0;
            }

    }

    if (matrix.x == -1){
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
        if (rank == 0) {
            if (human) print_time_human(end-start);
            else print_time_csv(end-start, matrix.x, size, stencil7);
        }
        // FIXME: CHECK FOR CORRECTNESS MANUALLY (Uncomment & Recompile)
        //MPI_Barrier(MPI_COMM_WORLD);
        //print_matrix_mpi(rank, size, &matrix);
    }

    if (run != 0) {
        destroy_matrix(&matrix);
    }

    MPI_Finalize();
    return 0;
}