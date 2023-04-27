#include <iostream>
#include <time.h>

using namespace std;

int main(int argc, char *argv[])
{
    int size = 3000;
    int element = 6;
    int *a = new int[size * size];
    int *b = new int[size * size];
    int *c = new int[size * size];
    clock_t start, finish;

    for (int i = 0; i < size; i++)
    {
        for (int j = 0; j < size; j++)
        {
            a[i * size + j] = element;
            b[i * size + j] = element + 2;
        }
    }

    start = clock();

    for (int i = 0; i < size; i++)
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

    finish = clock();
    printf(" time: %f s \n", double(finish - start) / CLOCKS_PER_SEC);

    FILE *fp = fopen("C.txt", "w");
    for (int i = 0; i < size; i++)
    {
        for (int j = 0; j < size; j++)
            fprintf(fp, "%d\t", c[i * size + j]);
        fputc('\n', fp);
    }
    fclose(fp);

    delete[] a, b, c;

    return 0;
}
