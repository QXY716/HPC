#include <iostream>
#include <mpi.h>
#include <pthread.h>
#define nthread 4

using namespace std;

struct thread_data
{
    int id;
    int line;
    int size;
    int *local_a;
    int *b;
    int *ans;
};

void *matrix_multiply(void *data)
{
    thread_data *td = (thread_data *)data;
    int id = td->id;
    int line = td->line;
    int size = td->size;
    int *local_a = td->local_a;
    int *b = td->b;
    int *ans = td->ans;

    for (int i = id * line; i < (id + 1) * line; i++)
    {
        for (int j = 0; j < size; j++)
        {
            int temp = 0;
            for (int k = 0; k < size; k++)
            {
                temp += local_a[i * size + k] * b[k * size + j];
            }
            ans[i * size + j] = temp;
        }
    }
    pthread_exit(NULL);
}

int main(int argc, char *argv[])
{
    int my_rank;
    int num_procs;
    int size = 3000;
    int element = 6;
    int i, j, k;
    double start, finish;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &num_procs);

    int line = size / num_procs;
    cout << " line = " << line << endl;
    int *local_a = new int[line * size];
    int *b = new int[size * size];
    int *ans = new int[line * size];
    int *a = new int[size * size];
    int *c = new int[size * size];

    if (my_rank == 0)
    {

        for (i = 0; i < size; i++)
        {
            for (j = 0; j < size; j++)
            {
                a[i * size + j] = element;
                b[i * size + j] = element + 2;
            }
        }

        start = MPI_Wtime();

        MPI_Scatter(a, line * size, MPI_INT, local_a, line * size, MPI_INT, 0, MPI_COMM_WORLD);

        MPI_Bcast(b, size * size, MPI_INT, 0, MPI_COMM_WORLD);

        pthread_t threads[nthread];
        thread_data td[nthread];
        int local_line = line / nthread;

        for (i = 0; i < nthread; i++)
        {
            td[i] = {i, local_line, size, local_a, b, ans};
            pthread_create(&threads[i], NULL, matrix_multiply, (void *)&td[i]);
        }

        for (i = 0; i < nthread; i++)
        {
            pthread_join(threads[i], NULL);
        }

        MPI_Gather(ans, line * size, MPI_INT, c, line * size, MPI_INT, 0, MPI_COMM_WORLD);

        for (i = num_procs * line; i < size; i++) 
        {
            for (j = 0; j < size; j++) {
                int temp = 0;
                for (k = 0; k < size; k++)
                    temp += a[i * size + k] * b[k * size + j];
                c[i * size + j] = temp;
            }
        }

        finish = MPI_Wtime();
        printf(" time: %lf s \n", finish - start);

        FILE *fp = fopen("C.txt", "w");
        for (i = 0; i < size; i++)
        {
            for (j = 0; j < size; j++)
                fprintf(fp, "%d\t", c[i * size + j]);
            fputc('\n', fp);
        }
        fclose(fp);
    }

    else
    {
        int *buffer = new int[size * line];
        MPI_Scatter(a, line * size, MPI_INT, buffer, line * size, MPI_INT, 0, MPI_COMM_WORLD);
        MPI_Bcast(b, size * size, MPI_INT, 0, MPI_COMM_WORLD);

        pthread_t threads[nthread];
        thread_data td[nthread];
        int local_line = line / nthread;

        for (i = 0; i < nthread; i++)
        {
            td[i] = {i, local_line, size, buffer, b, ans};
            pthread_create(&threads[i], NULL, matrix_multiply, (void *)&td[i]);
        }

        for (i = 0; i < nthread; i++)
        {
            pthread_join(threads[i], NULL);
        }

        MPI_Gather(ans, line * size, MPI_INT, c, line * size, MPI_INT, 0, MPI_COMM_WORLD);
        delete[] buffer;
    }

    delete[] a, local_a, b, ans, c;

    MPI_Finalize();
    return 0;
}