#include <stdio.h>

#include "main/c/core/core.h"

int main() {
    printf("Implement me!\n");
    struct pc_matrix matrix = parse("input.txt");

    switch (parcomp_parser_error) {
        case 1:
            printf("Parser works correctly with non-existing files!\n");
            break;
        case 2:
            printf("File is empty!\n");
            break;
        case 4:
            printf("Incorrect file format!\n");
            break;
        default:
            printf("No problem during parsing!\n");
    }

    destroy_matrix(&matrix);
    return 0;
}