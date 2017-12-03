/* File:    omp_trap2a.c
 * Purpose: Estimate definite integral (or area under curve) using trapezoidal
 *          rule.  This version uses a hand-coded reduction after the function
 *          call.
 *
 * Input:   a, b, n
 * Output:  estimate of integral from a to b of f(x)
 *          using n trapezoids.
 *
 * Compile: gcc -g -Wall -fopenmp -o omp_trap2a omp_trap2a.c -lm
 * Usage:   ./omp_trap2a <number of threads>
 *
 * Notes:
 *   1.  The function f(x) is hardwired.
 *   2.  This version assumes that n is evenly divisible by the
 *       number of threads
 * IPP:  Section 5.4 (p. 222)
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <omp.h>

int main(int argc, char* argv[]) {
   int     thread_count;

   thread_count = strtol(argv[1], NULL, 10);

   # pragma omp barrier
   double start = omp_get_wtime();

   # pragma omp parallel num_threads(thread_count)
   {
       int i ;
       double my_sum = 0.0;
       for ( i = 0; i < 100000 ; i ++)
       {
            # pragma omp atomic
            my_sum += sin(i);
        }
   }

   # pragma omp barrier
   double end = omp_get_wtime();

   printf("Elapsed time = %lf\n", end - start);
   return 0;
}  /* main */
