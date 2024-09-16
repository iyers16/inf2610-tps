/*
 * Ecole Polytechnique Montreal - GIGL
 * Automne 2024
 * Processes - part2.c
 *
 * Shanmukh Iyer 2251372; Michael Imbert 2212593
 */

#include "./libprocesslab/libprocesslab.h"

#define m 1000000
// question2 calcule la somme des m premiers nombres naturels 1+2+...+m

// nb est le nombre de threads qui vont contribuer au calcul
#define nb 4

// tableau somme est utilisé pour le calcul des sommes patielles par les threads
long somme[nb];

typedef struct
{
  int start;
  int end;
  int thread_num;
} thread_args_t;

// fonction exécutée par chaque thread créé
void *contribution(void *p)
{
  int tid = *(int *)p;
  int upper = (tid * m) / nb;
  int lower;
  if (tid == 1)
  {
    lower = 1;
  }
  else
  {
    lower = ((tid-1) * m) / nb;
  }
  int sum = 0;
  for (int i = lower; i <= upper; ++i)
  {
    sum += i;
  }
  somme[tid] = sum;
  printf("Sum of tid=%d is %ld\n", tid, somme[tid]);
  return NULL;
}

void question2()
{
  pthread_t t1, t2, t3, t4;
  int thread_ctr = 1;
  pthread_create(&t1, NULL, contribution, &thread_ctr);
  pthread_join(t1, NULL);
  ++thread_ctr;

  pthread_create(&t2, NULL, contribution, &thread_ctr);
  pthread_join(t2, NULL);
  ++thread_ctr;

  pthread_create(&t3, NULL, contribution, &thread_ctr);
  pthread_join(t3, NULL);
  ++thread_ctr;

  pthread_create(&t4, NULL, contribution, &thread_ctr);
  pthread_join(t4, NULL);

  
  unsigned long long realsum = (m * (m+1))/2;
  printf("Real sum = %llu\n", realsum);
  _exit(0);
}
