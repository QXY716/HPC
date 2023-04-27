#include <iostream>
#include <mpi.h>
#include <omp.h>

int main(int argc, char *argv[]) {
    int provided, required = MPI_THREAD_MULTIPLE;
    double start, finish;

    // 初始化MPI环境
    MPI_Init_thread(&argc, &argv, required, &provided);

    // 检查是否支持MPI_THREAD_MULTIPLE
    if (provided != required) {
        std::cerr << "Error: MPI implementation does not support MPI_THREAD_MULTIPLE" << std::endl;
        MPI_Abort(MPI_COMM_WORLD, 1);
    }

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    // 设定OpenMP线程数量
    omp_set_num_threads(4);
    start = MPI_Wtime();
    #pragma omp parallel
    {
        int thread_id = omp_get_thread_num();
        int num_threads = omp_get_num_threads();

        // 每个线程都有自己的发送和接收缓冲区
        int send_buf = rank * num_threads + thread_id;
        int recv_buf;

        // 阻塞发送和接收
        int source = (rank - 1 + size) % size;
        int dest = (rank + 1) % size;

        MPI_Sendrecv(&send_buf, 1, MPI_INT, dest, thread_id,
                     &recv_buf, 1, MPI_INT, source, thread_id, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        // 输出接收到的消息
//        #pragma omp critical
//        {
//            std::cout << "Rank " << rank << ", Thread " << thread_id << " received " << recv_buf << " from Rank " << source << ", Thread " << thread_id << std::endl;
//        }
    }

    finish = MPI_Wtime();
    if (rank == 0)
    {
        printf(" time: %lf s \n", finish - start);
    }

    // 结束MPI环境
    MPI_Finalize();
    return 0;
}
