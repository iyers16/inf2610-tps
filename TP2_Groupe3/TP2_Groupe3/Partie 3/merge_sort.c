#include "merge_sort.h"



int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <array_size> <num_processes>\n", argv[0]);
        exit(1);
    }

    int array_size = atoi(argv[1]);
    int num_processes = atoi(argv[2]);

    shared_data = mmap(NULL, sizeof(SharedData) + sizeof(int) * array_size, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    if (shared_data == MAP_FAILED) {
    perror("mmap");
    exit(1);
   }

    shared_data->size = array_size;
    shared_data->array = (int*)(shared_data + 1);
    
    /* Populate the array to test the sort */
    srand(time(NULL));
    for (int i = 0; i < array_size; i++) {
        shared_data->array[i] = rand() % MAX_NUM_SIZE;
    }

    execute_merge_sort(0, array_size - 1, num_processes);
    show_array();
    munmap(shared_data, sizeof(SharedData) + array_size * sizeof(int));

    return 0;
}

void merge_sort( int left, int right) {
    if (left < right) {
        int mid = left + (right - left) / 2;
        merge_sort(left, mid);
        merge_sort(mid + 1, right);
        merge(left, mid, right);
    }
}

void merge(int left, int mid, int right) {
    int i, j, k;
    int n1 = mid - left + 1;
    int n2 = right - mid;

    int L[n1], R[n2];

    for (i = 0; i < n1; i++)
        L[i] = shared_data->array[left + i];
    for (j = 0; j < n2; j++)
        R[j] = shared_data->array[mid + 1 + j];

    i = 0;
    j = 0;
    k = left;

    while (i < n1 && j < n2) {
        if (L[i] <= R[j]) {
            shared_data->array[k] = L[i];
            i++;
        } else {
            shared_data->array[k] = R[j];
            j++;
        }
        k++;
    }

    while (i < n1) {
        shared_data->array[k] = L[i];
        i++;
        k++;
    }

    while (j < n2) {
        shared_data->array[k] = R[j];
        j++;
        k++;
    }
}

void execute_merge_sort(int start, int end, int num_processes){
    int part_size = (end - start + 1) / num_processes; // taille de la partie du array dont le processus se charge

    // on separe en plusieurs partie pour chaque processus qui aura sa propre partie du array a merge_sort
    for (int i = 0; i < num_processes; i++){
        if(fork() == 0){
            int part_start = start + i * part_size;
            int part_end = ( i == num_processes - 1) ? end : (part_start + part_size - 1);
            merge_sort(part_start, part_end);
            exit(0);
        }
        while(wait(NULL) > 0);

        // pp doit fusionner les parties tries par les processus enfants
        pp_final_merge(start, end, num_processes, part_size);
    }

}

void pp_final_merge(int start, int end, int num_processes, int part_size) {
    int mid;
    for(int i = 1; i < num_processes; i ++) {
        mid = start + i * part_size - 1;
        int right = (i == num_processes - 1) ? end : (start + (i + 1) * part_size - 1);
        merge(start, mid, right);
    }
}

void show_array(){
    printf("Sorted array: ");
    for (int i = 0; i < shared_data->size; i++) {
        printf("%d ", shared_data->array[i]);
    }
    printf("\n");
}