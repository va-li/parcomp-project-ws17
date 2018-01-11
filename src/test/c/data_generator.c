/**
 * @file input_data_generator.c
 * @author Valentin Bauer (01425252)
 *
 * Generate random data to be used as input for the stencil problem
 * Options:
 *  - h : Help
 *  - o <filename> : name of the output file
 *  - x <X> : value of X dimension
 *  - y <Y> : value of Y dimension
 *  - z <Z> : value of Z dimension
 *  - p <processor count> : (Default 48) Must fulfill (X-2)*(Y-2)*(Z-2) % <processor count> = 0
 */
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <float.h>
#include <unistd.h>

extern char *optarg;
extern int optind, opterr, optopt;

/**
 * Generate a random floating point number from MIN to MAX
 * */
double rand_double(double min, double max);

/**
 * Print the usage message
 */
void print_usage();

int main(int argc, char **argv)
{
    int c;
    char *output_filename = "input.txt";
    int processor_count = 48;
    int x_dim = -1;
    int y_dim = -1;
    int z_dim = -1;


    while ((c = getopt(argc, argv, "o:p:x:y:z:")) != 1)
    {
        switch (c) {
            case o:

                break;
            default:
                print_usage();
                break;
        }
    }
}

double rand_double(double min, double max)
{
    double range = (max - min);
    double div = RAND_MAX / range;
    return min + (rand() / div);
}

void print_usage()
{
    (void) printf("Usage: data_generator [-h | -p <processor count> | -o <filename>] -x <X> -y <Y> -z <Z>");
}
