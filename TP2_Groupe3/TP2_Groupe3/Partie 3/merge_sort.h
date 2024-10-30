#include <sys/mman.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <semaphore.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <string.h>
#include <time.h>
#include <sys/time.h> 



#define MAX_ARRAY_SIZE 1000000
#define MAX_NUM_SIZE 100000
#define SEM_NAME "/merge_sort_sem"

#ifndef MAP_ANONYMOUS
#define MAP_ANONYMOUS 0x20
#endif

typedef struct {
    int* array;
    int size;
} SharedData;


SharedData *shared_data;
sem_t* mutex;
int fd;
struct timeval start_time, end_time;
double total_time;

void merge_sort( int left, int right);
void pp_final_merge(int start, int mid, int num_processes, int part_size);
void merge(int left, int mid, int right);
void execute_merge_sort(int start, int end, int num_processes);
void show_array();
void write_array_into_file(int start, int end, int *array);