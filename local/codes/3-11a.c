#include <stdio.h>
#include <time.h>
#include <stdlib.h>

int main(){
    int n = 15;
    srand(time(NULL));

    int vec[n];
    int prefix_sum[n];

    for(int i = 0; i < n; i++){
        vec[i] = rand() % 10; // number between 0 and 9
        printf("%d ", vec[i]);
    }
    printf("\n");

    for(int i = 0; i < n; i++){
        prefix_sum[i] = 0;
        for(int j = 0; j <= i; j++){
            prefix_sum[i] += vec[j];
        }
        printf("%d ", prefix_sum[i]);
    }
}
