#include <iostream>
#include <mpi.h>
#include <pthread.h>

using namespace std;

struct Args
{
    int line;
    int size;
    int my_rank;
    int num_procs;
    int *a;
    int *b;
    int *local_a;
    int *ans;
    int *c;
};

void *scatter_thread(void *args_ptr)
{
    Args *args = (Args *)args_ptr;
    int line = args->line;
    int size = args->size;
    int my_rank = args->my_rank;
    int num_procs = args->num_procs;
    int *a = args->a;
    int *local_a = args->local_a;

    MPI_Request request1[num_procs];

    if (my_rank == 0)
    {
        for (int p = 1; p < num_procs; ++p)
        {
            MPI_Isend(a + p * line * size, line * size, MPI_INT, p, 0, MPI_COMM_WORLD, &request1[p - 1]);
        }
    }
    else
    {
        MPI_Irecv(local_a, line * size, MPI_INT, 0, 0, MPI_COMM_WORLD, &request1[my_rank - 1]);
    }
//    MPI_Waitall(num_procs-1,request1, MPI_STATUS_IGNORE);
    return nullptr;
}

void *bcast_thread(void *args_ptr)
{
    Args *args = (Args *)args_ptr;
    int size = args->size;
    int *b = args->b;

    MPI_Request request;
    MPI_Ibcast(b, size * size, MPI_INT, 0, MPI_COMM_WORLD, &request);
    MPI_Wait(&request, MPI_STATUS_IGNORE);

    return nullptr;
}

void *gather_thread(void *args_ptr)
{
    Args *args = (Args *)args_ptr;
    int line = args->line;
    int size = args->size;
    int my_rank = args->my_rank;
    int num_procs = args->num_procs;
    int *ans = args->ans;
    int *c = args->c;

    MPI_Request request2[num_procs];

    if (my_rank == 0)
    {
        for (int p = 1; p < num_procs; ++p)
        {
            MPI_Irecv(c + p * line * size, line * size, MPI_INT, p, 1, MPI_COMM_WORLD, &request2[p - 1]);
        }
    }
    else
    {
        MPI_Isend(ans, line * size, MPI_INT, 0, 1, MPI_COMM_WORLD, &request2[my_rank - 1]);
    }
//    MPI_Waitall(num_procs-1,request2, MPI_STATUS_IGNORE);
    return nullptr;
}

int main(int argc, char *argv[])
{
    int my_rank;
    int num_procs;
    int size = 3000;
    int element = 6;
    int i, j, k;
    double start, finish;
    int provided;

    MPI_Init_thread(&argc, &argv, MPI_THREAD_MULTIPLE, &provided);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);   // Get the current process number
    MPI_Comm_size(MPI_COMM_WORLD, &num_procs); // Get the number of processes

    int line = size / num_procs; // Divide the data into (number of processes) blocks
    cout << " line = " << line << endl;
    int *local_a = new int[line * size]; // Matrix assigned to each process
    int *b = new int[size * size];
    int *ans = new int[line * size]; // The result of the data block calculation
    int *a = new int[size * size];
    int *c = new int[size * size];

    if (provided < MPI_THREAD_MULTIPLE)
    {
        cout << "MPI_THREAD_MULTIPLE not supported" << endl;
        MPI_Abort(MPI_COMM_WORLD, 1);
    }

    start = MPI_Wtime();

    pthread_t scatter, bcast, gather;
    Args args = {line, size, my_rank, num_procs, a, b, local_a, ans, c};

    if (my_rank == 0)
    {
        for (i = 0; i < size; i++)
        {
            for (j = 0; j < size; j++)
            {
                a[i * size + j] = element;
                b[i * size + j] = element + 2;
            }
        } // Create Matrix a and b

        pthread_create(&scatter, nullptr, scatter_thread, (void *)&args);
        pthread_create(&bcast, nullptr, bcast_thread, (void *)&args);
        pthread_create(&gather, nullptr, gather_thread, (void *)&args);
    }
    else
    {
        pthread_create(&scatter, nullptr, scatter_thread, (void *)&args);
        pthread_create(&bcast, nullptr, bcast_thread, (void *)&args);
        pthread_create(&gather, nullptr, gather_thread, (void *)&args);
    }
    
    // Wait for threads to finish
    pthread_join(scatter, nullptr);
    pthread_join(bcast, nullptr);

    if (my_rank == 0)
    {
        for (i = 0; i < line; i++)
        {
            for (j = 0; j < size; j++)
            {
                int temp = 0;
                for (k = 0; k < size; k++)
                    temp += a[i * size + k] * b[k * size + j];
                c[i * size + j] = temp;
            }
        }

        // Process remaining lines if size is not evenly divisible by num_procs
        for (int i = num_procs * line; i < size; i++)
        {
            for (int j = 0; j < size; j++)
            {
                int temp = 0;
                for (int k = 0; k < size; k++)
                {
                    temp += a[i * size + k] * b[k * size + j];
                }
                c[i * size + j] = temp;
            }
        }

        pthread_join(gather, nullptr);

        finish = MPI_Wtime();
        printf(" time: %lf s \n", finish - start);

        FILE *fp = fopen("C.txt", "w");
        for (int i = 0; i < size; i++)
        {
            for (int j = 0; j < size; j++)
            {
                fprintf(fp, "%d\t", c[i * size + j]);
            }
            fputc('\n', fp);
        }
        fclose(fp);
    }
    else
    {
        for (i = 0; i < line; i++)
        {
            for (j = 0; j < size; j++)
            {
                int temp = 0;
                for (k = 0; k < size; k++)
                    temp += local_a[i * size + k] * b[k * size + j];
                ans[i * size + j] = temp;
            }
        }
        pthread_join(gather, nullptr);
    }

    delete[] a, local_a, b, ans, c;

    MPI_Finalize();
    return 0;
}
