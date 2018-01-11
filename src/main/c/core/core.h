#ifndef PARCOMP_PROJECT_WS17_CORE_H
#define PARCOMP_PROJECT_WS17_CORE_H

//#define GET_PTR(arr, x, y, z)  (arr + x*y*z + x*y + x)
//#define GET_PTR(arr, x, y, z)  (arr + x * y * (z + 1) + x)

// Given a pointer of an array `arr` it returns the pointer of the element in <x,y,z>
#define GET_PTR(arr, x, y, z)  (arr + x * (y * (z + 1) + 1))

int parcomp_parser_error;

/**
 * This parses the file with the filename and returns the parsed array
 *
 * IF THERE'S AN ERROR DURING PARSING, `parcomp_parser_error` will be set to the corresponding values
 *      1 - can't open file
 *      2 - format is incorrect
 *
 * @param filename - the name of the file where the values are
 * @return the pointer to the array with parsed values
 */
int *parse(char* filename);

#endif //PARCOMP_PROJECT_WS17_CORE_H
