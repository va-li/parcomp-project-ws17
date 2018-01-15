#ifndef PARCOMP_PROJECT_WS17_UTILS_H
#define PARCOMP_PROJECT_WS17_UTILS_H

#include <stdbool.h>
/**
 * Prints the benchmarks in a human readable way
 * @param microseconds
 */
void print_benchmark_human(long microseconds, int xyz, int cores, bool stencil7, char* mode);

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
void print_benchmark_csv(long microseconds, int xyz, int cores, bool stencil7, char* mode);
#endif //PARCOMP_PROJECT_WS17_UTILS_H
