#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

int main(int argc, char *argv[])
{
    int rank, size, i;
    MPI_Datatype type;
    int n = 4;

    int blocklen[n], displacement[n];
    for (int i = n, j = 0;
         i > 0 && j < n; i--, j++)
         { blocklen[j] = i;}

    for (int i = 0, j = 0;
         i < n, j < n; i+=5, j++)
         { displacement[j] = i;}

    int buffer[n*n];
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (size < 2)
    {
        printf("Please run with 2 processes.\n");
        MPI_Finalize();
        return 1;
    }

    MPI_Type_indexed(n, blocklen, displacement, MPI_INT, &type);
    MPI_Type_commit(&type);

    if (rank == 0)
    {
        printf("Enter the matrix: ");
        fflush(stdout);
        for (int i = 0; i < n*n; i++){
            scanf("%d", &buffer[i]);
        }

        MPI_Send(buffer, 1, type, 1, 123, MPI_COMM_WORLD);
    }

    if (rank == 1)
    {
        for (i=0; i<n*n; i++)
            buffer[i] = -1;
        MPI_Recv(buffer, 1, type, 0, 123, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        for (i=0; i<n*n; i++)
            printf("buffer[%d] = %d\n", i, buffer[i]);
        fflush(stdout);
    }

    MPI_Finalize();
    return 0;
}
