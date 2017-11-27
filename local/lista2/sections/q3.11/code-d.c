#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <mpi.h>

void print_vec(int vec[], int n);

int main(void) {
    int comm_sz; /* Number of processes */
    int my_rank; /* My process rank */
    MPI_Init(NULL, NULL);
    MPI_Comm comm = MPI_COMM_WORLD;
    MPI_Comm_size(comm, &comm_sz);
    MPI_Comm_rank(comm, &my_rank);

    int count = comm_sz;
    int vec[count];
    int sum[count];

    srand(my_rank+1);
    for(int i = 0; i < count; i++){
        vec[i] = rand() % 10; // number between 0 and 9
        sum[i] = 0;
    }

    MPI_Scan(vec, sum, count, MPI_INT, MPI_SUM, comm);

    if (my_rank != 0) {
        MPI_Send(vec, count, MPI_INT, 0, 0, comm);
        MPI_Send(sum, count, MPI_INT, 0, 0, comm);
    } else {
        printf("rank %d - [", my_rank );
        print_vec(vec,count);
        printf("] => [");
        print_vec(sum,count);
        printf("]\n");
        for (int q = 1; q < comm_sz; q++) {
            MPI_Recv(vec, count, MPI_INT, q, 0, comm, MPI_STATUS_IGNORE);
            MPI_Recv(sum, count, MPI_INT, q, 0, comm, MPI_STATUS_IGNORE);
            printf("rank %d - [", q);
            print_vec(vec,count);
            printf("] => [");
            print_vec(sum,count);
            printf("]\n");
        }
    }

    MPI_Finalize();
    return 0;
}

void print_vec(int vec[], int n){
    for(int i = 0; i < n; i++){
        printf("%d ",vec[i]);
    }
}
