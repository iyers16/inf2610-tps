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
#define nb 8

// tableau somme est utilisé pour le calcul des sommes partielles par les threads
typedef unsigned long long ull_t;
ull_t somme[nb];

// fonction exécutée par chaque thread créé
void *contribution(void *p)
{
  int thread_id = *((int *)p);
  free(p);
  int segment = m/nb;
  int min_sum_term = ((thread_id - 1) * segment) + 1;
  int max_sum_term = thread_id * segment;
  ull_t partial_sum = ((ull_t)min_sum_term + (ull_t)max_sum_term) * segment / 2;
  printf("\nThread %d Partial sum =\t%llu\n",thread_id, partial_sum);
  somme[thread_id - 1] = partial_sum;
  return NULL;
}

void question2()
{
  pthread_t threads[nb];
  ull_t sum_total = 0;

  for (int i = 0; i < nb; i++)
  {
    int *thread_id = malloc(sizeof(int));
    *thread_id = i + 1;
    pthread_create(&threads[i], NULL, contribution, (void *)thread_id);
  }
  for (int i = 0; i < nb; i++)
  {
    pthread_join(threads[i], NULL);
  }

  for (int i = 0; i < nb; ++i)
  {
    sum_total += somme[i];
  }

  ull_t expected_sum_total = ((ull_t)m * (ull_t)(m+1))/2;

  printf("\nExpected: %llu\tResult: %llu\n", expected_sum_total, sum_total);
  return;
}
