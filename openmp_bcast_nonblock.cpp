#include <mpi.h>
#include <iostream>
#include <omp.h>
#define nthread 4

using namespace std;

int main(int argc, char *argv[])
{
    int i, rank, size;
    double start, finish;
    int provided;
    MPI_Init_thread(&argc, &argv, MPI_THREAD_MULTIPLE, &provided);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Request request[size-1];

{
    if (rank == 0)
    {
        int value = 42;
        start = MPI_Wtime();
#pragma omp parallel private(i) num_threads(nthread)
{
        #pragma omp for schedule(static)
        for (i = 1; i < size; i++)
        {
            MPI_Isend(&value, 1, MPI_INT, i, 0, MPI_COMM_WORLD, &request[i-1]);
        }
}
        MPI_Waitall(size - 1, request, MPI_STATUS_IGNORE);
        finish = MPI_Wtime();
        printf(" time: %lf s \n", finish - start);
        cout << "Process " << rank << " sent " << value << " to all processes" << endl;
    }
    else
    {
        int buf;
        MPI_Irecv(&buf, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &request[rank-1]);
        MPI_Wait(&request[rank-1], MPI_STATUS_IGNORE);
//        cout << "Process " << rank << " received " << value << endl;
    }
}

    MPI_Finalize();
    return 0;
}
