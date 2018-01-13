#include "openmp.h"

#include <stdio.h>
#include <omp.h>

void test() {
    int i = 25;
    printf("%d\n",i);

    #pragma omp parallel
    {
        int ID = omp_get_thread_num();
        printf("hello(%d) ", ID);
        printf("world(%d) \n", ID);
    }
}