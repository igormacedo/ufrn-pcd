#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <mpi.h>
#include <math.h>

void print_vec(int vec[], int n);
int main(){
    int comm_sz; /* Number of processes */
    int my_rank; /* My process rank */
    MPI_Init(NULL, NULL);
    MPI_Comm comm = MPI_COMM_WORLD;
    MPI_Comm_size(comm, &comm_sz);
    MPI_Comm_rank(comm, &my_rank);

    int vec_i;
    int n = comm_sz;
    int vec[n];

    if (my_rank == 0){
        srand(time(NULL));
        for(int i = 0; i < n; i++){
            vec[i] = 1; //rand() % 10; // number between 0 and 9
        }
        print_vec(vec,n);
        MPI_Scatter(vec, 1, MPI_INT, &vec_i, 1, MPI_INT, 0, comm);
    }
    else {
        MPI_Scatter(vec, 1, MPI_INT, &vec_i, 1, MPI_INT, 0, comm);
    }

    for(int k = 0; k < log2(n); k++){
        for(int i = pow(2,k) - 1; i < pow(2,log2(n)); i = i + pow(2,k+1)){
            for(int j = 1; j <= pow(2,k); j++){
                if (my_rank == i){
                    MPI_Send(&vec_i, 1, MPI_INT, i+j, 0, comm);
                    printf("This is %d, Sending to %d\n", my_rank, i+j);
                } else if (my_rank == i + j){
                    int aux_vec_i = 0;
                    printf("This is %d, Receiving from %d\n",my_rank,i);
                    MPI_Recv(&aux_vec_i, 1, MPI_INT, i, 0, comm, MPI_STATUS_IGNORE);
                    vec_i += aux_vec_i;
                }
            }
        }
    }

    int vec_sum[n];
    if (my_rank == 0) {
       MPI_Gather(&vec_i, 1, MPI_INT, vec_sum, 1, MPI_INT, 0, comm);
       print_vec(vec_sum,n);
    } else {
        MPI_Gather(&vec_i, 1, MPI_INT, vec_sum, 1, MPI_INT, 0, comm);
    }

    MPI_Finalize();
    return 0;
}

void print_vec(int vec[], int n){
    for(int i = 0; i < n; i++){
        printf("%d ",vec[i]);
    }
    printf("\n");
}
