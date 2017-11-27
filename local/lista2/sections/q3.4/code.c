#include <stdio.h>
#include <mpi.h>
#include <string.h> /* For strlen */

const int MAX_STRING = 100;
int main(void) {
    char phrase[MAX_STRING];
    int my_rank, comm_sz;

    MPI_Init(NULL, NULL);
    MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

    if (my_rank != 0) {
        sprintf(phrase, "Proc %d of %d > Does anyone have a toothpick?", my_rank, comm_sz);
        MPI_Send(phrase, strlen(phrase)+1, MPI_CHAR, 0, 0, MPI_COMM_WORLD);
    } else {
        printf("Proc %d of %d > Does anyone have a toothpick?\n", my_rank, comm_sz);
        for (int q = 1; q < comm_sz; q++) {
            MPI_Recv(phrase, MAX_STRING, MPI_CHAR, q, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            printf("%s\n",phrase);
        }
    }

    MPI_Finalize();
    return 0;
}  /* main */
