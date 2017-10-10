#include <iostream>
#include <time.h>   // clock()
#include <stdlib.h>     /* srand, rand */
#include <stdio.h>

using namespace std;

int a[256], b[256], c[256];

void foo()
{
  	int i;

  	for (i=0; i<256; i++){
    	a[i] = b[i] + c[i];
  	}
}

int main(int argc, char *argv[])
{    
	for(int i = 0; i < 256; i++)
	{
		a[i] = rand() % 10 + 1;
		b[i] = rand() % 10 + 1;
		c[i] = rand() % 10 + 1;
	}

	foo();
   	return 0;
}
