#include <iostream>
#include <time.h>
#include <pthread.h>
#define nthread 4
#define SIZE 3000

using namespace std;

int element = 6;
int* a = new int[SIZE * SIZE];
int* b = new int[SIZE * SIZE];
int* c = new int[SIZE * SIZE];

//pthread_mutex_t mutex;

void *threadFunc(void *arg) 
{
  int tid = (long)arg;

  for (int i = tid; i < SIZE; i += nthread)
  {
    for (int j = 0; j < SIZE; j++) 
    {
      int temp = 0;
      for (int k = 0; k < SIZE; k++) 
      {
        temp += a[i * SIZE + k] * b[k * SIZE + j];
      }
      c[i * SIZE + j] = temp;
    }
  }
  cout << "Thread " << tid << " finished multiplication." << endl;
  pthread_exit(NULL);
  //return NULL;
}

int main(int argc, char *argv[])
{
  for (int i = 0; i < SIZE; i++)
  {
    for (int j = 0; j < SIZE; j++)
    {
      a[i * SIZE + j] = element;
      b[i * SIZE + j] = element + 2;
    }
  }

  clock_t start, finish;

  start = clock();

  pthread_t threads[nthread];
  //pthread_mutex_init(&mutex, NULL);

  for (long i = 0; i < nthread; i++) 
  {
    pthread_create(&threads[i], NULL, threadFunc, (void *)i);
  }

  for (int i = 0; i < nthread; i++) 
  {
    pthread_join(threads[i], NULL);
  }

  finish = clock();

  printf(" time: %f s \n", double(finish - start) / CLOCKS_PER_SEC);

  FILE *fp = fopen("C.txt", "w");
  for (int i = 0; i < SIZE; i++)
  {
    for (int j = 0; j < SIZE; j++)
      fprintf(fp, "%d\t", c[i * SIZE + j]);
    fputc('\n', fp);
  }
  fclose(fp);

  delete[] a, b, c;

  return 0;
}
