
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
