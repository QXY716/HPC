#include <iostream>
#include <pthread.h>
#include <mpi.h>
#define nthread 4

struct ThreadData {
    int rank;
    int size;
    int thread_id;
};

void *send_receive(void *thread_data) 
{
    ThreadData *data = (ThreadData *)thread_data;
    int rank = data->rank;
    int size = data->size;
    int thread_id = data->thread_id;

    // 每个线程都有自己的发送和接收缓冲区
    int send_buf = rank * nthread + thread_id;
    int recv_buf;

    // 阻塞发送和接收
    int source = (rank - 1 + size) % size;
    int dest = (rank + 1) % size;
    MPI_Request send_request, recv_request;

    MPI_Sendrecv(&send_buf, 1, MPI_INT, dest, thread_id,
                 &recv_buf, 1, MPI_INT, source, thread_id, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

//    std::cout << "Rank " << rank << ", Thread " << thread_id << " received " << recv_buf << " from Rank " << source << ", Thread " << thread_id << std::endl;

    pthread_exit(NULL);
}

int main(int argc, char *argv[]) 
{
    double start, finish;
    int provided;
    MPI_Init_thread(&argc, &argv, MPI_THREAD_MULTIPLE, &provided);

    if (provided != MPI_THREAD_MULTIPLE) {
        std::cerr << "MPI_THREAD_MULTIPLE not supported!" << std::endl;
        MPI_Abort(MPI_COMM_WORLD, 1);
    }

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    start = MPI_Wtime();
    pthread_t threads[nthread];
    ThreadData thread_data[nthread];

    for (int i = 0; i < nthread; ++i) 
    {
        thread_data[i].rank = rank;
        thread_data[i].thread_id = i;
        thread_data[i].size = size;

        pthread_create(&threads[i], NULL, send_receive, (void *)&thread_data[i]);
    }

    for (int i = 0; i < nthread; ++i) 
    {
        pthread_join(threads[i], NULL);
    }

    finish = MPI_Wtime();

    if (rank==0)
    {
        printf(" time: %lf s \n", finish - start);
    }

    MPI_Finalize();
    return 0;
}
