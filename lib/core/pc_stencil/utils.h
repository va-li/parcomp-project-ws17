#ifndef PARCOMP_PROJECT_WS17_UTILS_H
#define PARCOMP_PROJECT_WS17_UTILS_H

#include <stdbool.h>

/**
 * Prints the benchmarks in a human readable way
 * @param microseconds
 */
void print_benchmark_human(long microseconds, int xyz, int cores, bool stencil7, char *mode);

/**
 * Prints the benchmark suitable to be saved in the csv format
 *      format: XxYxZ,stencil_type,MPI,cores,microseconds
 *
 *      ASSUMES X == Y == Z
 *
 * @param microseconds
 * @param xyz
 * @param cores
 * @param stencil7
 */
void print_benchmark_csv(long microseconds, int xyz, int cores, bool stencil7, char *mode);

/**
 * Prints a common usage for all single applications
 *  usage: ./pc_stencil_mpi filename [mode]
 *      only the first character is checked for the mode
 *  Modes:
 *          1 - Stencil7 csv
 *          2 - Stencil27 csv
 *          3 - Stencil7 human readable (default if empty)
 *          4 - Stencil27 human readable\n");
 */
void print_common_usage();

#endif //PARCOMP_PROJECT_WS17_UTILS_H
