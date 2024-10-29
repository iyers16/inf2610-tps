#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <signal.h>
#include <unistd.h>
#include <time.h>

#define MAX_BUFFER_SIZE 100

int *buffer;
int buffer_size;
int in = 0, out = 0;
int total_produced = 0, total_consumed = 0;
int sum_produced = 0, sum_consumed = 0;
int flag_de_fin = 0;

// Mutex and condition variables
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t buffer_not_full = PTHREAD_COND_INITIALIZER;
pthread_cond_t buffer_not_empty = PTHREAD_COND_INITIALIZER;

// Signal handler for SIGALRM
void alarm_handler(int sig) {
    flag_de_fin = 1;
}

// Producer function
void* producteur(void* pid) {
    int* producer_sum = malloc(sizeof(int));
    *producer_sum = 0;

    while (1) {
        if (flag_de_fin) break;

        // Generate a random number between 1 and 9
        int num = (rand() % 9) + 1;

        // Lock buffer and add item
        pthread_mutex_lock(&mutex);
        while ((in + 1) % buffer_size == out) {
            pthread_cond_wait(&buffer_not_full, &mutex);
        }

        // Insert item into buffer
        buffer[in] = num;
        in = (in + 1) % buffer_size;
        total_produced++;
        *producer_sum += num;
        sum_produced += num;

        // Signal to consumers
        pthread_cond_signal(&buffer_not_empty);
        pthread_mutex_unlock(&mutex);
    }

    pthread_exit((void*) producer_sum);
}

// Consumer function
void* consommateur(void* cid) {
    int* consumer_sum = malloc(sizeof(int));
    *consumer_sum = 0;

    while (1) {
        pthread_mutex_lock(&mutex);

        while (in == out) {
            pthread_cond_wait(&buffer_not_empty, &mutex);
        }

        // Remove item from buffer
        int num = buffer[out];
        out = (out + 1) % buffer_size;
        total_consumed++;
        *consumer_sum += num;
        sum_consumed += num;

        // Signal to producers
        pthread_cond_signal(&buffer_not_full);
        pthread_mutex_unlock(&mutex);

        if (num == 0) break; // Stop on receiving 0
    }

    pthread_exit((void*) consumer_sum);
}

int main(int argc, char* argv[]) {
    if (argc != 4) {
        fprintf(stderr, "Usage: %s <num_producers> <num_consumers> <buffer_size>\n", argv[0]);
        exit(1);
    }

    int num_producers = atoi(argv[1]);
    int num_consumers = atoi(argv[2]);
    buffer_size = atoi(argv[3]);

    if (buffer_size > MAX_BUFFER_SIZE) {
        fprintf(stderr, "Buffer size exceeds maximum allowed size.\n");
        exit(1);
    }

    // Initialize buffer
    buffer = malloc(buffer_size * sizeof(int));

    // Set up signal handling
    signal(SIGALRM, alarm_handler);
    alarm(1);

    // Create producer and consumer threads
    pthread_t producers[num_producers], consumers[num_consumers];
    int producer_ids[num_producers], consumer_ids[num_consumers];

    srand(time(NULL)); // Seed the random number generator

    for (int i = 0; i < num_producers; i++) {
        producer_ids[i] = i;
        pthread_create(&producers[i], NULL, producteur, &producer_ids[i]);
    }

    for (int i = 0; i < num_consumers; i++) {
        consumer_ids[i] = i;
        pthread_create(&consumers[i], NULL, consommateur, &consumer_ids[i]);
    }

    // Wait for producers to finish
    int total_produced_sum = 0;
    for (int i = 0; i < num_producers; i++) {
        int *prod_sum;
        pthread_join(producers[i], (void**) &prod_sum);
        total_produced_sum += *prod_sum;
        free(prod_sum);
    }

    // Send termination signals to consumers
    for (int i = 0; i < num_consumers; i++) {
        pthread_mutex_lock(&mutex);
        while ((in + 1) % buffer_size == out) {
            pthread_cond_wait(&buffer_not_full, &mutex);
        }
        buffer[in] = 0; // Signal consumer to stop
        in = (in + 1) % buffer_size;
        pthread_cond_signal(&buffer_not_empty);
        pthread_mutex_unlock(&mutex);
    }

    // Wait for consumers to finish
    int total_consumed_sum = 0;
    for (int i = 0; i < num_consumers; i++) {
        int *cons_sum;
        pthread_join(consumers[i], (void**) &cons_sum);
        total_consumed_sum += *cons_sum;
        free(cons_sum);
    }

    // Output results
    printf("Total produced sum: %d, Total consumed sum: %d\n", total_produced_sum, total_consumed_sum);
    printf("Total numbers produced: %d, Total numbers consumed: %d\n", total_produced, total_consumed);

    // Clean up
    free(buffer);
    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&buffer_not_full);
    pthread_cond_destroy(&buffer_not_empty);

    return 0;
}
