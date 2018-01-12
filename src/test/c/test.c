#include <stdio.h>

#include "main/c/core/core.h"

int main() {
    printf("Implement me!\n");
    struct pc_matrix matrix = parse("incorrect file");

    if (parcomp_parser_error != 0) {
        printf("Parser works correctly with non-existing files!\n");
    }
    return 0;
}