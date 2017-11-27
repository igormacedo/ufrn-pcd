/*Esse código foi modificado de mpi_vector_add.c */

/* File:     mpi_vector_add.c
 *
 * Purpose:  Implement parallel vector addition using a block
 *           distribution of the vectors.  This version also
 *           illustrates the use of MPI_Scatter and MPI_Gather.
 *
 * Compile:  mpicc -g -Wall -o mpi_vector_add mpi_vector_add.c
 * Run:      mpiexec -n <comm_sz> ./vector_add
 *
 * Input:    The order of the vectors, n, and the vectors x and y
 * Output:   The sum vector z = x+y
 *
 * Notes:
 * 1.  The order of the vectors, n, should be evenly divisible
 *     by comm_sz
 * 2.  DEBUG compile flag.
 * 3.  This program does fairly extensive error checking.  When
 *     an error is detected, a message is printed and the processes
 *     quit.  Errors detected are incorrect values of the vector
 *     order (negative or not evenly divisible by comm_sz), and
 *     malloc failures.
 *
 * IPP:  Section 3.4.6 (pp. 109 and ff.)
 */

#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

void Check_for_error(int local_ok, char fname[], char message[],
      MPI_Comm comm);
void Read_n(int* n_p, int* local_n_p, int* rest, int my_rank, int comm_sz,
      MPI_Comm comm);
void Allocate_vectors(double** local_x_pp, double** local_y_pp,
      double** local_z_pp, int local_n, MPI_Comm comm);
void Read_vector(int rest, double local_a[], int local_n, int n, char vec_name[],
      int my_rank, MPI_Comm comm);
void Read_scalar(double* scalar_p, int my_rank, int comm_sz, MPI_Comm comm);
void Print_vector(double local_b[], int local_n, int n, char title[],
      int my_rank, MPI_Comm comm);
void Parallel_vector_dotproduct(double local_x[], double local_y[],
      double local_z[], double* result, int local_n, int n, MPI_Comm comm);
void Parallel_scalar_mutiplication(double scalar, double local_x[],
    double local_z[], int local_n);


/*-------------------------------------------------------------------*/
int main(void) {
   int n, local_n, rest;
   int comm_sz, my_rank;
   double *local_x, *local_y, *local_z;
   double scalar, result;
   MPI_Comm comm;

   MPI_Init(NULL, NULL);
   comm = MPI_COMM_WORLD;
   MPI_Comm_size(comm, &comm_sz);
   MPI_Comm_rank(comm, &my_rank);

   Read_n(&n, &local_n, &rest, my_rank, comm_sz, comm);

   Allocate_vectors(&local_x, &local_y, &local_z, local_n, comm);

   Read_vector(rest, local_x, local_n, n, "x", my_rank, comm);
   Print_vector(local_x, local_n, n, "x is", my_rank, comm);
   printf("Done ok" );
   Read_vector(rest, local_y, local_n, n, "y", my_rank, comm);
   Print_vector(local_y, local_n, n, "y is", my_rank, comm);

   Read_scalar(&scalar, my_rank, comm_sz, comm);

   Parallel_scalar_mutiplication(scalar, local_x, local_x, local_n);
   Print_vector(local_x, local_n, n, "now x is", my_rank, comm);
   Parallel_vector_dotproduct(local_x, local_y, local_z, &result, local_n,
                                n, comm);

   if (my_rank == 0) {
      printf("The result of (scalar*x[]).y[] is %lf \n", result);
   }

   free(local_x);
   free(local_y);
   free(local_z);

   MPI_Finalize();

   return 0;
}  /* main */

/*-------------------------------------------------------------------
 * Function:  Check_for_error
 * Purpose:   Check whether any process has found an error.  If so,
 *            print message and terminate all processes.  Otherwise,
 *            continue execution.
 * In args:   local_ok:  0 if calling process has found an error, 1
 *               otherwise
 *            fname:     name of function calling Check_for_error
 *            message:   message to print if there's an error
 *            comm:      communicator containing processes calling
 *                       Check_for_error:  should be MPI_COMM_WORLD.
 *
 * Note:
 *    The communicator containing the processes calling Check_for_error
 *    should be MPI_COMM_WORLD.
 */
void Check_for_error(
      int       local_ok   /* in */,
      char      fname[]    /* in */,
      char      message[]  /* in */,
      MPI_Comm  comm       /* in */) {
   int ok;

   /* Pega o mínimo do vetor
        Se o mínimo for zero, aconteceu algum erro,
        caso contrário, todos os processos retornaram 1 e estão ok
   */
   MPI_Allreduce(&local_ok, &ok, 1, MPI_INT, MPI_MIN, comm);
   if (ok == 0) {
      int my_rank;
      MPI_Comm_rank(comm, &my_rank);
      if (my_rank == 0) {
         fprintf(stderr, "Proc %d > In %s, %s\n", my_rank, fname,
               message);
         fflush(stderr);
      }
      MPI_Finalize();
      exit(-1);
   }
}  /* Check_for_error */


/*-------------------------------------------------------------------
 * Function:  Read_n
 * Purpose:   Get the order of the vectors from stdin on proc 0 and
 *            broadcast to other processes.
 * In args:   my_rank:    process rank in communicator
 *            comm_sz:    number of processes in communicator
 *            comm:       communicator containing all the processes
 *                        calling Read_n
 * Out args:  n_p:        global value of n
 *            local_n_p:  local value of n = n/comm_sz
 *
 * Errors:    n should be positive and evenly divisible by comm_sz
 */
void Read_n(
      int*      n_p        /* out */,
      int*      local_n_p  /* out */,
      int*      rest       /* out */,
      int       my_rank    /* in  */,
      int       comm_sz    /* in  */,
      MPI_Comm  comm       /* in  */) {
   int local_ok = 1;
   char *fname = "Read_n";

   if (my_rank == 0) {
      printf("What's the order of the vectors?\n");
      scanf("%d", n_p);
   }
   MPI_Bcast(n_p, 1, MPI_INT, 0, comm);
   if (*n_p <= 0 ) local_ok = 0;
   Check_for_error(local_ok, fname,
         "n should be > 0. ", comm);
   *local_n_p = *n_p/comm_sz + 1;
   *rest = *n_p % comm_sz;
}  /* Read_n */


/*-------------------------------------------------------------------
 * Function:  Allocate_vectors
 * Purpose:   Allocate storage for x, y, and z
 * In args:   local_n:  the size of the local vectors
 *            comm:     the communicator containing the calling processes
 * Out args:  local_x_pp, local_y_pp, local_z_pp:  pointers to memory
 *               blocks to be allocated for local vectors
 *
 * Errors:    One or more of the calls to malloc fails
 */
void Allocate_vectors(
      double**   local_x_pp  /* out */,
      double**   local_y_pp  /* out */,
      double**   local_z_pp  /* out */,
      int        local_n     /* in  */,
      MPI_Comm   comm        /* in  */) {
   int local_ok = 1;
   char* fname = "Allocate_vectors";

   *local_x_pp = malloc(local_n*sizeof(double));
   *local_y_pp = malloc(local_n*sizeof(double));
   *local_z_pp = malloc(local_n*sizeof(double));

   if (*local_x_pp == NULL || *local_y_pp == NULL ||
       *local_z_pp == NULL) local_ok = 0;
   Check_for_error(local_ok, fname, "Can't allocate local vector(s)",
         comm);
}  /* Allocate_vectors */


/*-------------------------------------------------------------------
 * Function:   Read_vector
 * Purpose:    Read a vector from stdin on process 0 and distribute
 *             among the processes using a block distribution.
 * In args:    local_n:  size of local vectors
 *             n:        size of global vector
 *             vec_name: name of vector being read (e.g., "x")
 *             my_rank:  calling process' rank in comm
 *             comm:     communicator containing calling processes
 * Out arg:    local_a:  local vector read
 *
 * Errors:     if the malloc on process 0 for temporary storage
 *             fails the program terminates
 *
 * Note:
 *    This function assumes a block distribution and the order
 *   of the vector evenly divisible by comm_sz.
 */
void Read_vector(
      int       rest        /* in  */,
      double    local_a[]   /* out */,
      int       local_n     /* in  */,
      int       n           /* in  */,
      char      vec_name[]  /* in  */,
      int       my_rank     /* in  */,
      MPI_Comm  comm        /* in  */) {

   double* a = NULL;
   int i;
   int local_ok = 1;
   char* fname = "Read_vector";

   int comm_sz;
   MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);
   int* sendcounts = malloc(sizeof(int)*comm_sz);
   int* displs = malloc(sizeof(int)*comm_sz);

   // Calculating send counts and displacement
   int sum = 0;
   int rem = rest;
    for (int i = 0; i < comm_sz; i++) {
        sendcounts[i] = n/comm_sz;
        if (rem > 0) {
            sendcounts[i]++;
            rem--;
        }

        displs[i] = sum;
        sum += sendcounts[i];
    }

    // print calculated send counts and displacements for each process
    if (0 == my_rank) {
       for (int i = 0; i < comm_sz; i++) {
           printf("sendcounts[%d] = %d\tdispls[%d] = %d\n", i, sendcounts[i], i, displs[i]);
       }
    }

    a = malloc(n*sizeof(double));
    if (a == NULL) local_ok = 0;
    Check_for_error(local_ok, fname, "Can't allocate temporary vector",
          comm);

    if (my_rank == 0) {
      printf("Enter the vector %s\n", vec_name);
      for (i = 0; i < n; i++)
         scanf("%lf", &a[i]); // reads a double (long float)
    }

    // divide the data among processes as described by sendcounts and displs
    // MPI_Scatterv(&data, sendcounts, displs, MPI_CHAR, &rec_buf, 100, MPI_CHAR, 0, MPI_COMM_WORLD);
    MPI_Scatterv(a, sendcounts, displs, MPI_DOUBLE, local_a, local_n, MPI_DOUBLE, 0, comm);

    // print what each process received
     printf("%d: ", my_rank);
     for (int i = 0; i < sendcounts[my_rank]; i++) {
         printf("%lf\t", local_a[i]);
     }
     printf("\n");

     free(a);
     free(sendcounts);
     free(displs);
}  /* Read_vector */

/*-------------------------------------------------------------------
 * Function:  Read_scalar
 * Purpose:   Get the the scalar number from stdin on proc 0 and
 *            broadcast to other processes.
 * In args:   my_rank:    process rank in communicator
 *            comm_sz:    number of processes in communicator
 *            comm:       communicator containing all the processes
 *                        calling Read_n
 * Out args:  scalar_p:        global value of n
 *
 */
void Read_scalar(
      double*   scalar_p        /* out */,
      int       my_rank    /* in  */,
      int       comm_sz    /* in  */,
      MPI_Comm  comm       /* in  */) {

   if (my_rank == 0) {
      printf("What's the scalar value?\n");
      scanf("%lf", scalar_p); // reads double
   }
   MPI_Bcast(scalar_p, 1, MPI_DOUBLE, 0, comm);

}  /* Read_scalar */

/*-------------------------------------------------------------------
 * Function:  Print_vector
 * Purpose:   Print a vector that has a block distribution to stdout
 * In args:   local_b:  local storage for vector to be printed
 *            local_n:  order of local vectors
 *            n:        order of global vector (local_n*comm_sz)
 *            title:    title to precede print out
 *            comm:     communicator containing processes calling
 *                      Print_vector
 *
 * Error:     if process 0 can't allocate temporary storage for
 *            the full vector, the program terminates.
 *
 * Note:
 *    Assumes order of vector is evenly divisible by the number of
 *    processes
 */
void Print_vector(
      double    local_b[]  /* in */,
      int       local_n    /* in */,
      int       n          /* in */,
      char      title[]    /* in */,
      int       my_rank    /* in */,
      MPI_Comm  comm       /* in */) {

    double* b = NULL;
    int i;
    int local_ok = 1;
    char* fname = "Print_vector";

    int comm_sz;
    MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);
    int* recvcounts = malloc(sizeof(int)*comm_sz);
    int* displs = malloc(sizeof(int)*comm_sz);

    // Calculating send counts and displacement
    int sum = 0;
    int rem = n%comm_sz;
    for (int i = 0; i < comm_sz; i++) {
        recvcounts[i] = n/comm_sz;
        if (rem > 0) {
            recvcounts[i]++;
            rem--;
        }

        displs[i] = sum;
        sum += recvcounts[i];
    }

    if (0 == my_rank) {
       for (int i = 0; i < comm_sz; i++) {
           printf("recvcounts[%d] = %d\tdispls[%d] = %d\n", i, recvcounts[i], i, displs[i]);
       }
    }


    b = malloc(n*sizeof(double));
    if (b == NULL) local_ok = 0;
    Check_for_error(local_ok, fname, "Can't allocate temporary vector",
          comm);

   if (my_rank == 0) {
      MPI_Gatherv(local_b, recvcounts[my_rank], MPI_DOUBLE, b, recvcounts, displs,
           MPI_DOUBLE, 0, comm);
      printf("%s\n", title);
      for (i = 0; i < n; i++)
         printf("%f ", b[i]);
      printf("\n");

   } else {
      MPI_Gatherv(local_b, recvcounts[my_rank], MPI_DOUBLE, b, recvcounts, displs,
              MPI_DOUBLE, 0, comm);
   }

   free(b);
}  /* Print_vector */


/*-------------------------------------------------------------------
 * Function:  Parallel_vector_dotproduct
 * Purpose:   Add a vector that's been distributed among the processes
 * In args:   local_x:  local storage of one of the vectors being added
 *            local_y:  local storage for the second vector being added
 *            local_n:  the number of components in local_x, local_y,
 *                      and local_z
 * Out arg:   local_z:  local storage for the sum of the two vectors
 */
void Parallel_vector_dotproduct(
      double  local_x[]  /* in  */,
      double  local_y[]  /* in  */,
      double  local_z[]  /* out */,
      double* result     /* out */,
      int     local_n    /* in  */,
      int     n          /* in  */,
      MPI_Comm  comm     /* in */) {
   int local_i;

  for (local_i = 0; local_i < local_n; local_i++){
      local_z[local_i] = local_x[local_i] * local_y[local_i];
  }

  double local_total_z = 0.0;
  for (local_i = 0; local_i < local_n; local_i++){
      local_total_z += local_z[local_i];
  }

  int my_rank;
  MPI_Comm_rank(comm, &my_rank);
  printf("rank: %d > %lf\n", my_rank, local_total_z );

  MPI_Reduce(&local_total_z, result, 1, MPI_DOUBLE, MPI_SUM, 0, comm);

}  /* Parallel_vector_dotproduct */

/*-------------------------------------------------------------------
 * Function:  Parallel_scalar_mutiplication
 * Purpose:   Add a vector that's been distributed among the processes
 * In args:   scalar:   storage for the scalar value
 *            local_x:  local storage for the second vector being added
 *            local_n:  the number of components in local_x, local_y,
 *                      and local_z
 * Out arg:   local_z:  local storage for the scalar mutiplication of
 *                      the vector
 */
void Parallel_scalar_mutiplication(
      double  scalar     /* in  */,
      double  local_x[]  /* in  */,
      double  local_z[]  /* out */,
      int     local_n    /* in  */) {
   int local_i;

   for (local_i = 0; local_i < local_n; local_i++)
      local_z[local_i] = local_x[local_i] * scalar;
}  /* Parallel_scalar_mutiplication */
