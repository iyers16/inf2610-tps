#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <signal.h>
#include <unistd.h>
#include <time.h>
#include <stdbool.h>
#include <semaphore.h>

#define WRAP_AROUND(index, size) ((index + 1) % size)

volatile int *bounded_buffer;
int buff_size;
volatile long long int prod_sum = 0;
volatile long long int cons_sum = 0;
volatile int prod_idx = 0;
volatile int cons_idx = 0;
volatile int prod_ctr = 0;
volatile int cons_ctr = 0;
volatile bool flag_de_fin = false;

// Sémaphores pour la synchronisation
sem_t sem_empty; // Compte les cases vides dans le buffer
sem_t sem_full;  // Compte les cases pleines dans le buffer
sem_t sem_mutex; // Contrôle l'accès exclusif au buffer

void handle_alarm(int sig)
{
    flag_de_fin = true;
}

void *producteur(void *pid)
{
    int *id = (int *)pid;
    int *local_sum = malloc(sizeof(int));
    *local_sum = 0;
    while (true)
    {
        sem_wait(&sem_empty);
        if (flag_de_fin)
        {
            sem_post(&sem_empty);
            break;
        }
        sem_wait(&sem_mutex);
        int val = (rand() % 9) + 1;
        bounded_buffer[prod_idx] = val;
        prod_sum += val;
        *local_sum += val;
        prod_idx = WRAP_AROUND(prod_idx, buff_size);
        prod_ctr++;
        sem_post(&sem_mutex);
        sem_post(&sem_full);
    }

    pthread_exit((void *)local_sum);
}

void *consommateur(void *cid)
{
    int *id = (int *)cid;
    int *local_sum = malloc(sizeof(int));
    *local_sum = 0;

    while (true)
    {
        sem_wait(&sem_full);
        sem_wait(&sem_mutex);
        int val = bounded_buffer[cons_idx];
        cons_sum += val;
        *local_sum += val;
        cons_idx = WRAP_AROUND(cons_idx, buff_size);
        cons_ctr++;
        sem_post(&sem_mutex);
        sem_post(&sem_empty);
        if (val == 0)
        {
            break;
        }
    }

    pthread_exit((void *)local_sum);
}

int main(int argc, char *argv[])
{
    if (argc != 4)
    {
        perror("\nUsage: ./GuerreDesChiffres <num_producers> <num_consumers> <buffer_size>\n");
        _exit(1);
    }
    const int producers = atoi(argv[1]);
    const int consumers = atoi(argv[2]);
    buff_size = atoi(argv[3]);

    bounded_buffer = (int *) malloc(buff_size * sizeof(int));
    pthread_t *prod_arr = (pthread_t *) malloc(producers * sizeof(pthread_t));
    pthread_t *cons_arr = (pthread_t *) malloc(consumers * sizeof(pthread_t));
    sem_init(&sem_empty, 0, buff_size); // Commence avec toutes les cases vides
    sem_init(&sem_full, 0, 0);          // Commence sans cases pleines
    sem_init(&sem_mutex, 0, 1);         // Sémaphore binaire pour l'accès exclusif au buffer

    signal(SIGALRM, handle_alarm);
    alarm(1);

    for (int i = 0; i < producers; i++)
    {
        int *id = malloc(sizeof(int));
        *id = i;
        pthread_create(&prod_arr[i], NULL, producteur, (void *) id);
    }

    for (int i = 0; i < consumers; i++)
    {
        int *id = malloc(sizeof(int));
        *id = i;
        pthread_create(&cons_arr[i], NULL, consommateur, (void *) id);
    }

    long long total_prod_sum = 0;
    for (int i = 0; i < producers; i++)
    {
        int *local_sum;
        pthread_join(prod_arr[i], (void **) &local_sum);
        total_prod_sum += *local_sum;
        free(local_sum);
    }

    for (int i = 0; i < consumers; i++)
    {
        sem_wait(&sem_empty);

        sem_wait(&sem_mutex);
        bounded_buffer[prod_idx] = 0;
        prod_idx = WRAP_AROUND(prod_idx, buff_size);
        sem_post(&sem_mutex);
        sem_post(&sem_full);
    }

    long long total_cons_sum = 0;
    for (int i = 0; i < consumers; i++)
    {
        int *local_sum;
        pthread_join(cons_arr[i], (void **) &local_sum);
        total_cons_sum += *local_sum;
        free(local_sum);
    }

    printf("Total produced sum: %lld\n", total_prod_sum);
    printf("Total consumed sum: %lld\n", total_cons_sum);
    printf("Total produced count: %d\n", prod_ctr);
    printf("Total consumed count: %d\n", cons_ctr);

    // Cleanup
    free((void *) bounded_buffer);
    free(prod_arr);
    free(cons_arr);
    sem_destroy(&sem_empty);
    sem_destroy(&sem_full);
    sem_destroy(&sem_mutex);

    return 0;
}