#include <stdio.h>
#include <stdlib.h>

void Print_two_d(int two_d[][], int rows, int cols);

int main(){
    int two_d[3][4] = {{1,1,1,1},
                       {2,2,2,2},
                       {3,3,3,3}};

   Print_two_d(two_d,3,4);
   return 0;
}

void Print_two_d(int two_d[][], int rows, int cols){
    int i, j;
    for(i = 0; i < rows; i++){
        for(j = 0; j < cols; j++){
            printf("%d\n", two_d[i][j]);
        }
    }
}

#include <stdio.h>
#include <stdlib.h>

void Print_two_d(int two_d[][4], int rows, int cols);

int main(){
    int two_d[3][4] = {{1,1,1,1},
                       {2,2,2,2},
                       {3,3,3,3}};

   Print_two_d(two_d,3,4);
   return 0;
}

void Print_two_d(int two_d[][4], int rows, int cols){
    int i, j;
    for(i = 0; i < rows; i++){
        for(j = 0; j < cols; j++){
            printf("%d\n", two_d[i][j]);
        }
    }
}
