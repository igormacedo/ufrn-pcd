/* File:     mpi_mat_vect_time.c
 *
 * Purpose:  Implement parallel matrix-vector multiplication using
 *           one-dimensional arrays to store the vectors and the
 *           matrix.  Vectors use block distributions and the
 *           matrix is distributed by block rows.  This version
 *           generates a random matrix A and a random vector x.
 *           It prints out the run-time.
 *
 * Compile:  mpicc -g -Wall -o mpi_mat_vect_time mpi_mat_vect_time.c
 * Run:      mpiexec -n <number of processes> ./mpi_mat_vect_time
 *
 * Input:    Dimensions of the matrix (m = number of rows, n
 *              = number of columns)
 * Output:   Elapsed time for execution of the multiplication
 *
 * Notes:
 *    1. Number of processes should evenly divide both m and n
 *    2. Define DEBUG for verbose output, including the product
 *       vector y
 *
 * IPP:  Section 3.6.2 (pp. 122 and ff.)
 */
#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

void Check_for_error(int local_ok, char fname[], char message[],
      MPI_Comm comm);
void Get_dims(int* m_p, int* local_m_p, int* n_p, int* local_n_p,
      int my_rank, int comm_sz, MPI_Comm comm);
void Allocate_arrays(double** local_A_pp, double** local_x_pp,
      double** local_y_pp, double** local_answer, double** vec_answer,
      int m, int local_m, int n, int local_n, MPI_Comm comm);
void Read_matrix(char prompt[], double local_A[], int m, int local_m,
      int n, int my_rank, MPI_Comm comm);
void Read_vector(char prompt[], double local_vec[], int n, int local_n,
      int my_rank, MPI_Comm comm);
void Generate_matrix(char prompt[], double local_A[], int m, int local_m,
      int n, int local_n, int my_rank, MPI_Comm comm);
void Generate_vector(char prompt[], double local_vec[], int n, int local_n,
      int my_rank, MPI_Comm comm);
void Print_matrix(char title[], double local_A[], int m, int local_m,
      int n, int local_n, int my_rank, MPI_Comm comm);
void Print_vector(char title[], double local_vec[], int n,
      int local_n, int my_rank, MPI_Comm comm);
void Mat_vect_mult(double local_A[], double local_x[],
      double local_y[], double local_answer[], int m, int local_m, int n,
      int local_n, int my_rank, MPI_Comm comm);

MPI_Datatype col, matrix_column;
/*-------------------------------------------------------------------*/
int main(void) {
   double* local_A;
   double* local_x;
   double* local_y;
   double* local_answer;
   double* vec_answer;
   int m, local_m, n, local_n;
   int my_rank, comm_sz;
   MPI_Comm comm;
   //double start, finish, loc_elapsed, elapsed;

   MPI_Init(NULL, NULL);
   comm = MPI_COMM_WORLD;
   MPI_Comm_size(comm, &comm_sz);
   MPI_Comm_rank(comm, &my_rank);
   srandom(0);

   Get_dims(&m, &local_m, &n, &local_n, my_rank, comm_sz, comm);

   // criando datatype
   MPI_Type_vector(m, 1, n, MPI_DOUBLE, &col);
   MPI_Type_commit(&col);
   MPI_Type_create_resized(col, 0, 1*sizeof(double), &matrix_column);
   MPI_Type_commit(&matrix_column);

   Allocate_arrays(&local_A, &local_x, &local_y, &local_answer, &vec_answer,
                    m, local_m, n, local_n, comm);
//   Read_matrix("A", local_A, m, local_m, n, my_rank, comm);
   Generate_matrix("A", local_A, m, local_m, n, local_n, my_rank, comm);
//#  ifdef DEBUG
   Print_matrix("A", local_A, m, local_m, n, local_n, my_rank, comm);
//#  endif
//   Read_vector("x", local_x, n, local_n, my_rank, comm);
   Generate_vector("x", local_x, n, local_n, my_rank, comm);
//#  ifdef DEBUG
   Print_vector("x", local_x, n, local_n, my_rank, comm);
//#  endif

// int i = 0;
// for(i = 0; i < 100; i++){
//     MPI_Barrier(comm);
//     start = MPI_Wtime();
   Mat_vect_mult(local_A, local_x, local_y, local_answer, m, local_m, n, local_n,
       my_rank, comm);
   MPI_Reduce(local_answer, vec_answer, m, MPI_DOUBLE, MPI_SUM, 0, comm);
   //finish = MPI_Wtime();
   //loc_elapsed = finish-start;
   //MPI_Reduce(&loc_elapsed, &elapsed, 1, MPI_DOUBLE, MPI_MAX, 0, comm);

//    #ifdef DEBUG
//    Print_vector("y", local_y, m, local_m, my_rank, comm);
//    #endif

//     if (my_rank == 0)
//        printf("%e,", elapsed);
// }

    if (my_rank == 0)
    {
       printf("Answer: \n");
       int i;
       for (i = 0; i < m; i++)
           printf("%f ", vec_answer[i]);
    }

   free(local_A);
   free(local_x);
   free(local_y);
   free(local_answer);
   free(vec_answer);
   MPI_Finalize();
   return 0;
}  /* main */


/*-------------------------------------------------------------------*/
void Check_for_error(
      int       local_ok   /* in */,
      char      fname[]    /* in */,
      char      message[]  /* in */,
      MPI_Comm  comm       /* in */) {
   int ok;

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

/*-------------------------------------------------------------------*/
void Get_dims(
      int*      m_p        /* out */,
      int*      local_m_p  /* out */,
      int*      n_p        /* out */,
      int*      local_n_p  /* out */,
      int       my_rank    /* in  */,
      int       comm_sz    /* in  */,
      MPI_Comm  comm       /* in  */) {
   int local_ok = 1;

   if (my_rank == 0) {
      //printf("Enter the number of rows\n");
      //scanf("%d", m_p);
      *m_p = 8;
      //printf("Enter the number of columns\n");
      //scanf("%d", n_p);
      *n_p = 8;
      printf("Col: %d and Lin: %d\n", *m_p, *n_p);
   }
   MPI_Bcast(m_p, 1, MPI_INT, 0, comm);
   MPI_Bcast(n_p, 1, MPI_INT, 0, comm);
   if (*m_p <= 0 || *n_p <= 0 || *m_p % comm_sz != 0
         || *n_p % comm_sz != 0) local_ok = 0;
   Check_for_error(local_ok, "Get_dims",
      "m and n must be positive and evenly divisible by comm_sz",
      comm);

   *local_m_p = *m_p/comm_sz;
   *local_n_p = *n_p/comm_sz;
}  /* Get_dims */

/*-------------------------------------------------------------------*/
void Allocate_arrays(
      double**  local_A_pp  /* out */,
      double**  local_x_pp  /* out */,
      double**  local_y_pp  /* out */,
      double**  local_answer_pp  /* out */,
      double**  vec_answer_pp  /* out */,
      int       m           /* in  */,
      int       local_m     /* in  */,
      int       n           /* in  */,
      int       local_n     /* in  */,
      MPI_Comm  comm        /* in  */) {

   int local_ok = 1;

   *local_A_pp = malloc(local_n*m*sizeof(double));
   *local_x_pp = malloc(local_n*sizeof(double));
   *local_y_pp = malloc(local_n*m*sizeof(double));
   *local_answer_pp = malloc(m*sizeof(double));
   *vec_answer_pp = malloc(m*sizeof(double));

   if (*local_A_pp == NULL || local_x_pp == NULL ||
         local_y_pp == NULL) local_ok = 0;
   Check_for_error(local_ok, "Allocate_arrays",
         "Can't allocate local arrays", comm);
}  /* Allocate_arrays */

/*-------------------------------------------------------------------*/
void Read_matrix(
      char      prompt[]   /* in  */,
      double    local_A[]  /* out */,
      int       m          /* in  */,
      int       local_m    /* in  */,
      int       n          /* in  */,
      int       my_rank    /* in  */,
      MPI_Comm  comm       /* in  */) {
   double* A = NULL;
   int local_ok = 1;
   int i, j;

   if (my_rank == 0) {
      A = malloc(m*n*sizeof(double));
      if (A == NULL) local_ok = 0;
      Check_for_error(local_ok, "Read_matrix",
            "Can't allocate temporary matrix", comm);
      printf("Enter the matrix %s\n", prompt);
      for (i = 0; i < m; i++)
         for (j = 0; j < n; j++)
            scanf("%lf", &A[i*n+j]);
      MPI_Scatter(A, local_m*n, MPI_DOUBLE,
            local_A, local_m*n, MPI_DOUBLE, 0, comm);
      free(A);
   } else {
      Check_for_error(local_ok, "Read_matrix",
            "Can't allocate temporary matrix", comm);
      MPI_Scatter(A, local_m*n, MPI_DOUBLE,
            local_A, local_m*n, MPI_DOUBLE, 0, comm);
   }
}  /* Read_matrix */

/*-------------------------------------------------------------------*/
void Read_vector(
      char      prompt[]     /* in  */,
      double    local_vec[]  /* out */,
      int       n            /* in  */,
      int       local_n      /* in  */,
      int       my_rank      /* in  */,
      MPI_Comm  comm         /* in  */) {
   double* vec = NULL;
   int i, local_ok = 1;

   if (my_rank == 0) {
      vec = malloc(n*sizeof(double));
      if (vec == NULL) local_ok = 0;
      Check_for_error(local_ok, "Read_vector",
            "Can't allocate temporary vector", comm);
      printf("Enter the vector %s\n", prompt);
      for (i = 0; i < n; i++)
         scanf("%lf", &vec[i]);
      MPI_Scatter(vec, local_n, MPI_DOUBLE,
            local_vec, local_n, MPI_DOUBLE, 0, comm);
      free(vec);
   } else {
      Check_for_error(local_ok, "Read_vector",
            "Can't allocate temporary vector", comm);
      MPI_Scatter(vec, local_n, MPI_DOUBLE,
            local_vec, local_n, MPI_DOUBLE, 0, comm);
   }
}  /* Read_vector */

/*-------------------------------------------------------------------*/
void Generate_matrix(
    char      prompt[]   /* in  */,
    double    local_A[]  /* out */,
    int       m          /* in  */,
    int       local_m    /* in  */,
    int       n          /* in  */,
    int       local_n    /* in  */,
    int       my_rank    /* in  */,
    MPI_Comm  comm       /* in  */) {

     double* A = NULL;
     int local_ok = 1;
     int i, j;

     if (my_rank == 0) {
        A = malloc(m*n*sizeof(double));
        if (A == NULL) local_ok = 0;
        Check_for_error(local_ok, "Read_matrix",
              "Can't allocate temporary matrix", comm);
        printf("Generating random matrix %s... \n", prompt);
        for (i = 0; i < m; i++)
           for (j = 0; j < n; j++)
              A[i*n+j] = ((double) random())/((double) RAND_MAX);
        MPI_Scatter(A, local_n, matrix_column,
              local_A, local_n*m, MPI_DOUBLE, 0, comm);
        free(A);
     } else {
        Check_for_error(local_ok, "Read_matrix",
              "Can't allocate temporary matrix", comm);
        MPI_Scatter(A, local_n, matrix_column,
              local_A, local_n*m, MPI_DOUBLE, 0, comm);
     }

     if (my_rank == 0)
     {
        printf("Elements on process 0 after scatter \n");
        for (i = 0; i < local_n*m; i++)
            printf("%f ", local_A[i]);
     }

}  /* Generate_matrix */

/*-------------------------------------------------------------------*/
void Generate_vector(
    char      prompt[]     /* in  */,
    double    local_vec[]  /* out */,
    int       n            /* in  */,
    int       local_n      /* in  */,
    int       my_rank      /* in  */,
    MPI_Comm  comm         /* in  */) {

     double* vec = NULL;
     int i, local_ok = 1;

     if (my_rank == 0) {
        vec = malloc(n*sizeof(double));
        if (vec == NULL) local_ok = 0;
        Check_for_error(local_ok, "Read_vector",
              "Can't allocate temporary vector", comm);
        printf("Generating random vector %s... \n", prompt);
        for (i = 0; i < n; i++)
           vec[i] = ((double) random())/((double) RAND_MAX);
        MPI_Scatter(vec, local_n, MPI_DOUBLE,
              local_vec, local_n, MPI_DOUBLE, 0, comm);
        free(vec);
     } else {
        Check_for_error(local_ok, "Read_vector",
              "Can't allocate temporary vector", comm);
        MPI_Scatter(vec, local_n, MPI_DOUBLE,
              local_vec, local_n, MPI_DOUBLE, 0, comm);
     }

}  /* Generate_vector */

/*-------------------------------------------------------------------*/
void Print_matrix(
      char      title[]    /* in */,
      double    local_A[]  /* in */,
      int       m          /* in */,
      int       local_m    /* in */,
      int       n          /* in */,
      int       local_n    /* in */,
      int       my_rank    /* in */,
      MPI_Comm  comm       /* in */) {
   double* A = NULL;
   int i, j, local_ok = 1;

   if (my_rank == 0) {
      A = malloc(m*n*sizeof(double));
      if (A == NULL) local_ok = 0;
      Check_for_error(local_ok, "Print_matrix",
            "Can't allocate temporary matrix", comm);
      MPI_Gather(local_A, local_n*m, MPI_DOUBLE,
            A, local_n, matrix_column, 0, comm);
      printf("\nThe matrix %s\n", title);
      for (i = 0; i < m; i++) {
         for (j = 0; j < n; j++)
            printf("%f ", A[i*n+j]);
         printf("\n");
      }
      printf("\n");
      free(A);
   } else {
      Check_for_error(local_ok, "Print_matrix",
            "Can't allocate temporary matrix", comm);
      MPI_Gather(local_A, local_n*m, MPI_DOUBLE,
            A, local_n, matrix_column, 0, comm);
   }
}  /* Print_matrix */

/*-------------------------------------------------------------------*/
void Print_vector(
      char      title[]     /* in */,
      double    local_vec[] /* in */,
      int       n           /* in */,
      int       local_n     /* in */,
      int       my_rank     /* in */,
      MPI_Comm  comm        /* in */) {
   double* vec = NULL;
   int i, local_ok = 1;

   if (my_rank == 0) {
      vec = malloc(n*sizeof(double));
      if (vec == NULL) local_ok = 0;
      Check_for_error(local_ok, "Print_vector",
            "Can't allocate temporary vector", comm);
      MPI_Gather(local_vec, local_n, MPI_DOUBLE,
            vec, local_n, MPI_DOUBLE, 0, comm);
      printf("\nThe vector %s\n", title);
      for (i = 0; i < n; i++)
         printf("%f ", vec[i]);
      printf("\n");
      free(vec);
   }  else {
      Check_for_error(local_ok, "Print_vector",
            "Can't allocate temporary vector", comm);
      MPI_Gather(local_vec, local_n, MPI_DOUBLE,
            vec, local_n, MPI_DOUBLE, 0, comm);
   }
}  /* Print_vector */

/*-------------------------------------------------------------------*/
void Mat_vect_mult(
      double    local_A[]  /* in  */,
      double    local_x[]  /* in  */,
      double    local_y[]  /* out */,
      double    local_answer[]  /* out */,
      int       m          /* in  */,
      int       local_m    /* in  */,
      int       n          /* in  */,
      int       local_n    /* in  */,
      int       my_rank,
      MPI_Comm  comm       /* in  */) {
   double* x;
   int local_i, j;
   int local_ok = 1;

   x = malloc(n*sizeof(double));
   if (x == NULL) local_ok = 0;
   Check_for_error(local_ok, "Mat_vect_mult",
         "Can't allocate temporary vector", comm);
   MPI_Allgather(local_x, local_n, MPI_DOUBLE,
         x, local_n, MPI_DOUBLE, comm);

   for (local_i = 0; local_i < local_n; local_i++) {
      for (j = 0; j < m; j++)
         // mutiplica a linha da matriz vezes a coluna do vetor
         local_y[local_i*m+j] = local_A[local_i*m+j]*x[my_rank*local_n+local_i];
   }

   // if (my_rank == 0)
   // {
   //    printf("\nLocal_y: \n");
   //    for (local_i = 0; local_i < local_n; local_i++) {
   //       for (j = 0; j < m; j++){
   //          printf("%f ", local_y[local_i*m+j]);
   //       }
   //    }
   //    printf("\n\n");
   // }

   for (j = 0; j < m; j++){
      local_answer[j] = 0.0;
      for (local_i = 0; local_i < local_n; local_i++) {
         // adiciona todos os elementos da linha do vetor local_y
         // para fazer o mpi_reduce depois
         local_answer[j] += local_y[local_i*m+j];
     }
   }

   // if (my_rank == 0)
   // {
   //    printf("Local_answer: \n");
   //    int i;
   //    for (i = 0; i < m; i++){
   //      printf("%f ", local_answer[i]);
   //    }
   //    printf("\n\n");
   // }

   free(x);
}  /* Mat_vect_mult */
