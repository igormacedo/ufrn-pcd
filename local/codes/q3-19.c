#include <stdio.h>
#include <mpi.h> /* For MPI functions, etc */

const int MAX_STRING = 100;
int main(void) {
    char greeting[MAX_STRING];
    int comm_sz; /* Number of processes */
    int my_rank; /* My process rank */

    MPI_Init(NULL, NULL);
    MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

    if (comm_sz != 2){
        printf("Please run with 2 processes.\n");
        MPI_Finalize();
        return 1;
    }

    int n = 4;
    int m[n*n], blocklengths[n], displacements[n];

    for (int i = n, j = 0;
         i > 0 && j < n; i--, j++)
         { blocklengths[j] = i;}

    for (int i = 0, j = 0;
         i < n, j < n; i+=5, j++)
         { displacements[j] = i;}

    MPI_Datatype UPPER;
    MPI_Type_indexed(n, blocklengths, displacements,
                     MPI_INT, &UPPER);
    MPI_Type_commit(&UPPER);

    if (my_rank == 0) {
        printf("Enter the matrix: ");
        for (int i = 0; i < n*n; i++)
        {scanf("%d ", &m[i]);} // reads a double (long float)

        MPI_Send(m, 1, UPPER, 1, 0, MPI_COMM_WORLD);
    }
    if (my_rank == 1) {
        MPI_Recv(m, 1, UPPER, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        printf("Received data: ");
        for (int i = 0; i < n; i++)
           printf("%d ", m[i]);
    }

    MPI_Finalize();
    return 0;
}
