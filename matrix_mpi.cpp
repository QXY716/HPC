#include<iostream>
#include<mpi.h>

using namespace std;

int main(int argc, char *argv[]) {

    int my_rank;
    int num_procs;
    int size = 3000;
    int element = 6;
    int i,j,k;
    double start, finish;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank); //Get the current process number
    MPI_Comm_size(MPI_COMM_WORLD, &num_procs); //Get the number of processes

    int line = size / num_procs; //Divide the data into (number of processes) blocks
    cout << " line = " << line << endl;
    int* local_a = new int[line * size]; //Matrix assigned to each process
    int* b = new int[size * size];
    int* ans = new int[line * size]; //The result of the data block calculation
    int* a = new int[size * size];
    int* c = new int[size * size];

    if (my_rank == 0) {

        for (i = 0; i < size; i++) {
            for (j = 0; j < size; j++) {
                a[i * size + j] = element;
                b[i * size + j] = element + 2;
            }
        } //Create Matrix a and b

        start = MPI_Wtime();

        MPI_Scatter(a, line * size, MPI_INT, local_a, line * size, MPI_INT, 0, MPI_COMM_WORLD);

        MPI_Bcast(b, size * size, MPI_INT, 0, MPI_COMM_WORLD);

        for (i = 0; i < line; i++) {
            for (j = 0; j < size; j++) {
                int temp = 0;
                for (k = 0; k < size; k++)
                    temp += a[i * size + k] * b[k * size + j];
                ans[i * size + j] = temp;
            }
        }

        MPI_Gather(ans, line * size, MPI_INT, c, line * size, MPI_INT, 0, MPI_COMM_WORLD);

        for (i = num_procs * line; i < size; i++) {
            for (j = 0; j < size; j++) {
                int temp = 0;
                for (k = 0; k < size; k++)
                    temp += a[i * size + k] * b[k * size + j];
                c[i * size + j] = temp;
            }
        }

        finish = MPI_Wtime();
        printf(" time: %lf s \n", finish - start);

        FILE* fp = fopen("C.txt", "w");
        for (i = 0; i < size; i++) {
            for (j = 0; j < size; j++)
                fprintf(fp, "%d\t", c[i * size + j]);
            fputc('\n', fp);
        }
        fclose(fp);

    }

    else {
        int* buffer = new int[size * line];
        MPI_Scatter(a, line * size, MPI_INT, buffer, line * size, MPI_INT, 0, MPI_COMM_WORLD);
        MPI_Bcast(b, size * size, MPI_INT, 0, MPI_COMM_WORLD);
        

        for (i = 0; i < line; i++) {
            for (j = 0; j < size; j++) {
                int temp = 0;
                for (k = 0; k < size; k++)
                    temp += buffer[i * size + k] * b[k * size + j];
                ans[i * size + j] = temp;
            }
        }

        MPI_Gather(ans, line * size, MPI_INT, c, line * size, MPI_INT, 0, MPI_COMM_WORLD);
        delete[] buffer;
    }

    delete[] a, local_a, b, ans, c;

    MPI_Finalize();
    return 0;
}
