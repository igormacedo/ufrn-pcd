#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <math.h>

int main(){
    int n = 8;
    srand(time(NULL));

    int vec[n];

    for(int i = 0; i < n; i++){
        vec[i] = 1; //rand() % 10; // number between 0 and 9
        printf("%d ", vec[i]);
    }
    printf("\n");

    for(int k = 0; k < log2(n); k++){
        printf("k is %d\n",k);
        for(int i = pow(2,k) - 1; i < pow(2,log2(n)); i = i + pow(2,k+1)){
            printf("i is %d\n",i);
            for(int j = 1; j <= pow(2,k); j++){
                printf("vec[%d] = vec[%d] + vec[%d]\n",i+j,i,i+j);
                vec[i+j] = vec[i] + vec[i+j];
            }
        }
        for(int i = 0; i < n; i++){
            printf("%d ", vec[i]);
        }
        printf("\n");
    }

    for(int i = 0; i < n; i++){
        printf("%d ", vec[i]);
    }
    printf("\n");
}
