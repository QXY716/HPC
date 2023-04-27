#include <mpi.h>
#include <iostream>

int main(int argc, char **argv)
{
    int rank, size, number;
    MPI_Request request;
    double start, finish;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (rank == 0)
    {
        number = 42;
    }

    start = MPI_Wtime();
    MPI_Ibcast(&number, 1, MPI_INT, 0, MPI_COMM_WORLD, &request);

    MPI_Wait(&request, MPI_STATUS_IGNORE);

    finish = MPI_Wtime();

    if (rank == 0)
    {
        printf(" time: %lf s \n", finish - start);
    }

//    std::cout << "Process " << rank << ": The number is " << number << std::endl;

    MPI_Finalize();
    return 0;
}
