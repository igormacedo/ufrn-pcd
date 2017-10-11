#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <mpi.h>

int main(){
    int comm_sz; /* Number of processes */
    int my_rank; /* My process rank */
    MPI_Init(NULL, NULL);
    MPI_Comm comm = MPI_COMM_WORLD;
    MPI_Comm_size(comm, &comm_sz);
    MPI_Comm_rank(comm, &my_rank);

    int vec_i;
    int prefix_sum = 0;
    int n = comm_sz;
    int vec[n];

    if (my_rank == 0){
        srand(time(NULL));
        for(int i = 0; i < n; i++){
            vec[i] = rand() % 10; // number between 0 and 9
            printf("%d ", vec[i]);
        }
        printf("\n");

        MPI_Scatter(vec, 1, MPI_INT, &vec_i, 1, MPI_INT, 0, comm);
    }
    else {
        MPI_Scatter(vec, 1, MPI_INT, &vec_i, 1, MPI_INT, 0, comm);
    }

    // envia para todos os processo maiores do que ele mesmo
    for(int i = my_rank; i < comm_sz; i++){
        MPI_Send(&vec_i, 1, MPI_INT, i, 0, comm);
    }

    // recebe de todos os processos menores que ele mesmo
    prefix_sum += vec_i;
    for(int i = 0; i < my_rank; i++){
        int aux_vec_i = 0;
        MPI_Recv(&aux_vec_i, 1, MPI_INT, i, 0, comm, MPI_STATUS_IGNORE);
        prefix_sum += aux_vec_i;
    }


    // Imprime o resultado na tela
    if (my_rank != 0) {
        MPI_Send(&prefix_sum, 1, MPI_INT, 0, 1, comm);
    } else {
        printf("%d ", vec_i);
        for (int q = 1; q < comm_sz; q++) {
            int aux;
            MPI_Recv(&aux, 1, MPI_INT, q, 1, comm, MPI_STATUS_IGNORE);
            printf("%d ", aux);
        }
        printf("\n");
    }


    MPI_Finalize();
    return 0;
}
