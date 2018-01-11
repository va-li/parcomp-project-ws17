#include <stdio.h>
#include "core.h"

parcomp_parser_error = 0;

int *parse(char* filename) {
    FILE *f = fopen(filename, "r");

    if (f == NULL) {
        parcomp_parser_error = 1;
        return NULL;
    }

    return NULL;
}