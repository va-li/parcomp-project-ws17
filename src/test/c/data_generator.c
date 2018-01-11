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
#include <string.h>

#define MAX_FILENAME_CHARS (256)

extern char *optarg;
extern int optind, opterr, optopt;

/**
 * Generate a random floating point number between MIN and MAX
 * @param min
 * @param max
 * @return
 */
static double rand_double(double min, double max);

/**
 * Print the usage message
 */
static void print_usage();

int main(int argc, char **argv)
{
    int c;
    char *output_filename = "input.txt";
    long processor_count = 48;
    long x_dim = -1;
    long y_dim = -1;
    long z_dim = -1;


    while ((c = getopt(argc, argv, "o:p:x:y:z:")) != 1)
    {
        switch (c)
        {
            case 'o':
                (void) strncpy(output_filename, optarg, strnlen(optarg, MAX_FILENAME_CHARS));
                break;
            case 'p':
                p = strtol(optarg, NULL, 10);
                break;
            case 'x':
                x_dim = strtol(optarg, NULL, 10);
                break;
            case 'y':
                y_dim = strtol(optarg, NULL, 10);
                break;
            case 'z':
                z_dim = strtol(optarg, NULL, 10);
                break;
            default:
                print_usage();
                break;
        }
    }

    if ((x_dim % p != 0)
        || (y_dim % p != 0)
        || (z_dim % p != 0))
    {
        fprintf(stderr, "Not all dimensions are evenly divisible by the number of processors!\n");
        return -1;
    }

    FILE *fp = fopen("./input.txt", w);
    (void) fprintf(fp, "%dl;%dl;%dl\n", x_dim, y_dim, z_dim);

    long value_count = x_dim*y_dim*z_dim;

    for (int i = 0; i<value_count; i++) {
        (void) fprintf("%f\n", rand_double(-DBL_MAX, DBL_MAX));
    }

    (void) fclose(fp);

    return 0;
}

static double rand_double(double min, double max)
{
    double range = (max - min);
    double div = RAND_MAX / range;
    return min + (rand() / div);
}

static void print_usage()
{
    (void) printf("Usage: data_generator [-h | -p <processor count> | -o <filename>] -x <X> -y <Y> -z <Z>");
}
