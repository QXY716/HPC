#include <mpi.h>
#include <iostream>
#include <omp.h>
#define nthread 4

using namespace std;

int main(int argc, char *argv[]) 
{
    int i, rank, size;
    int provided;
    double start, finish;
    MPI_Init_thread(&argc, &argv, MPI_THREAD_MULTIPLE, &provided);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (rank == 0) 
    {
        int value = 42;
        start = MPI_Wtime();
#pragma omp parallel private(i) num_threads(nthread)
{
        #pragma omp for schedule(static)
        for (i = 1; i < size; i++) 
        {
            MPI_Send(&value, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
        }
}
        finish = MPI_Wtime();
        cout << "Process " << rank << " sent " << value << " to all processes" << endl;
        printf(" time: %lf s \n", finish - start);
    } 
    else 
    {
        int buf;
        MPI_Recv(&buf, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        //cout << "Process " << rank << " received " << value << endl;
    }
    MPI_Finalize();
    return 0;
}
