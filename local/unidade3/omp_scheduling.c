/* File:    omp_trap1.c
 * Purpose: Print default scheduling.
 *
 * Input:   a, b, n
 * Output:  estimate of integral from a to b of f(x)
 *          using n trapezoids.
 *
 * Compile: gcc -g -Wall -fopenmp -o omp_trap1 omp_trap1.c
 * Usage:   ./omp_trap1 <number of threads>
 *
 * IPP:  Section 5.2.1 (pp. 216 and ff.)
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <omp.h>

void Usage(char* prog_name);

int main(int argc, char* argv[]) {
    int     thread_count;
    int     iterations;

    if (argc != 3) Usage(argv[0]);

    thread_count = strtol(argv[1], NULL, 10);
    iterations = strtol(argv[2], NULL, 10);

#   pragma omp parallel num_threads(thread_count)
    {
        int i;
        int bottom = iterations;
        int top = 0;

#       pragma omp for
        for(i = 0; i < iterations; i++)
        {
            if(i < bottom) bottom = i;
            if(i > top) top = i;
        }

        int my_rank = omp_get_thread_num();
#       pragma omp for ordered
        for(i = 0; i < thread_count; i++)
        {
#           pragma omp ordered
            if( i == my_rank)
            printf("Thread %d: Iterations %d -- %d\n", my_rank, bottom, top);

        }

    }
    return 0;
}  /* main */

/*--------------------------------------------------------------------
 * Function:    Usage
 * Purpose:     Print command line for function and terminate
 * In arg:      prog_name
 */
void Usage(char* prog_name) {

   fprintf(stderr, "usage: %s <number of threads> <number of iterations>\n", prog_name);
   fprintf(stderr, "   number of trapezoids must be evenly divisible by\n");
   fprintf(stderr, "   number of threads\n");
   exit(0);
}  /* Usage */
