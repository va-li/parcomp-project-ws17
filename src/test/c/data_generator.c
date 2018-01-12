/**
 * @file input_data_generator.c
 * @author Valentin Bauer (01425252)
 *
 * Generate random data to be used as input for the stencil problem
 * Options:
 *  - h : Help
 *  - o <filename> : name of the output file
 *  - n <X> : value of N dimension
 *  - m <Y> : value of M dimension
 *  - l <Z> : value of L dimension
 *  - p <processor count> : (Default 48) Must fulfill (X-2)*(Y-2)*(Z-2) % <processor count> = 0
 */
#include <stdio.h>
#include <stdlib.h>
#include <float.h>
#include <unistd.h>
#include <string.h>
#include <time.h>

#define MAX_FILENAME_CHARS (256)

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

static void init_rand();

int main(int argc, char **argv) {
    int c;
    char *output_filename = "input.txt";
    long processor_count = 48;
    long n_dim = -1;
    long m_dim = -1;
    long l_dim = -1;


    while ((c = getopt(argc, argv, "ho:p:x:y:z:")) != -1) {
        switch (c) {
            case 'o':
                (void) strncpy(output_filename, optarg, strnlen(optarg, MAX_FILENAME_CHARS));
                break;
            case 'p':
                processor_count = strtol(optarg, NULL, 10);
                break;
            case 'x':
                n_dim = strtol(optarg, NULL, 10);
                break;
            case 'y':
                m_dim = strtol(optarg, NULL, 10);
                break;
            case 'z':
                l_dim = strtol(optarg, NULL, 10);
                break;
            case 'h':
            case '?':
            default:
                print_usage();
                return 0;
        }
    }

    if (((n_dim+2) % processor_count != 0)
        || ((m_dim+2)% processor_count != 0)
        || ((l_dim+2) % processor_count != 0)) {
        fprintf(stderr, "Not all dimensions are evenly divisible by the number of processors!\n");
        return -1;
    }

    FILE *fp = fopen("./input.txt", "w");
    (void) fprintf(fp, "%li;%li;%li\n", n_dim+2, m_dim+2, l_dim+2);

    long value_count = n_dim+2 * m_dim+2 * l_dim+2;

    init_rand();

    for (int i = 0; i < value_count; i++) {
        (void) fprintf(fp, "%f\n", rand_double(-1.0, 1));
    }

    (void) fclose(fp);

    return 0;
}

static double rand_double(double min, double max) {
    double range = (max - min);
    double div = RAND_MAX / range;
    return min + (rand() / div);
}

static void print_usage() {
    (void) printf("Usage: data_generator [-h | -p <processor count> | -o <filename>] -n <N> -m <M> -l <L>");
}

static void init_rand() {
    srand(time(NULL));
}
