#include <stdio.h>

#include "main/c/core/core.h"
#include "main/c/sequential/sequential.h"

int main() {
    struct pc_matrix matrix = parse("input.txt");

    switch (parcomp_parser_error) {
        case 1:
            printf("Error: can't open the file.\n");
            break;
        case 2:
            printf("Error: file is empty.\n");
            break;
        case 4:
            printf("Error: could not parse x,y,z.\n");
            break;
        case 8:
            printf("Error: number of lines is not equals x*y*z.\n");
            break;
        default:
            printf("The file is parsed successfully.\n");
    }

    run_stencil_7(&matrix);

    for (int i = 0; i < matrix.z; i++) {
        for (int j = 0; j < matrix.x * matrix.y; j++) {
            printf("%.3f\n",matrix.arr[i][j]);
        }
    }

    //run_stencil_27(&matrix);

    destroy_matrix(&matrix);
    return 0;
}