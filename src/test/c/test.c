#include <stdio.h>

#include "main/c/core/core.h"

int main() {
    printf("Implement me!\n");
    int *array = parse("incorrect file");

    if (array == (void *) 0) {
        printf("Parser works correctly with non-existing files!\n");
    }
    return 0;
}