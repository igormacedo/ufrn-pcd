# ufrn-pcd
## Preparing environment ##
```
$ sudo apt-get install libopenmpi-dev
$ mpiexec  -n  4  ./mpi_hello
```

## Compile and execute MPI programs ##
```
$ mpicc  -std=c99 -g  -Wall  -o  mpi_hello  mpi_hello.c
$ mpiexec  -n  4  ./mpi_hello
```
