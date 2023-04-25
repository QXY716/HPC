#include <iostream>
#include <time.h>
#include <omp.h>
#define nthread 2
using namespace std;

int main(int argc, char *argv[])
{
    int size = 3000;
    int element = 6;
    int* a = new int[size * size];
    int* b = new int[size * size];
    int* c = new int[size * size];
    int i,j,k;
    clock_t start, finish;

    for (i = 0; i < size; i++) 
    {
        for (j = 0; j < size; j++) 
        {
            a[i * size + j] = element;
            b[i * size + j] = element + 2;
        }
    }

    start = clock();

    omp_set_num_threads(nthread);
#pragma omp parallel shared(a,b,c) private(i,j,k)
{
    #pragma omp for schedule(dynamic)
    for (i = 0; i < size; i++) 
    {
        for (j = 0; j < size; j++) 
        {
            int temp = 0;
            for (k = 0; k < size; k++)
            {
                temp += a[i * size + k] * b[k * size + j];
            }
            c[i * size + j] = temp;
        }
    }
}

    finish = clock();
    printf(" time: %f s \n", double(finish - start)/CLOCKS_PER_SEC);

    FILE* fp = fopen("C.txt", "w");
    for (i = 0; i < size; i++) 
    {
        for (j = 0; j < size; j++)
            fprintf(fp, "%d\t", c[i * size + j]);
        fputc('\n', fp);
    }
    fclose(fp);

    delete[] a, b, c;

    return 0;
}
