#include <iostream>
#include  <time.h>   // clock()
#include <stdlib.h>     /* srand, rand */

//compile with

using namespace std;

int main( int argc, char *argv[])
{
    
	int r, c, i, j, k;
	r = atoi(argv[1]);
	c = atoi(argv[1]);


    /*cout << "Enter rows and columns for first matrix: ";
    cin >> r1 >> c1;
    cout << "Enter rows and columns for second matrix: ";
    cin >> r2 >> c2;*/
	
	cout << "Creating two square matrices (" << r <<","<< c <<")  and mutiplying them.";
	
	int a[r][c], b[r][c], mult[c][r];
	clock_t clock_1, clock_2;

	/* initialize random seed: */
  	srand (time(NULL));


    // Storing elements of first matrix.
    cout << endl << "Creating elements of matrix 1" << endl;
    for(i = 0; i < r; ++i)
 	{
        for(j = 0; j < c; ++j)
        {
            //cout << "Enter element a" << i + 1 << j + 1 << " : ";
            a[i][j] = rand() % 10 + 1;
			b[i][j] = rand() % 10 + 1;
			mult[i][j]=0;
        }
	}


    // Multiplying matrix a and b and storing in array mult.
	clock_1 = clock();
    for(i = 0; i < r; ++i)
	{
        for(j = 0; j < c; ++j)
		{
            for(k = 0; k < c; ++k)
            {
                mult[i][j] += a[i][k] * b[k][j];
            }
		}
	}
	clock_2 = clock();

    // Displaying the multiplication of two matrix.
    cout << endl << "Matrix multiplication done! " << endl;
	cout << "Tempo de utilização de CPU em segundos: " << (double)(clock_2-clock_1)/(double)CLOCKS_PER_SEC << endl;
    
/*for(i = 0; i < r; ++i)
    for(j = 0; j < c; ++j)
    {
        cout << " " << mult[i][j];
        if(j == c2-1)
            cout << endl;
    }*/

    return 0;
}
