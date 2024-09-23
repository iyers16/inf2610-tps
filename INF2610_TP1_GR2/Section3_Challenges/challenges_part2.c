/*
 * Ecole Polytechnique Montreal - GIGL
 * Automne 2024
 * Challenges - part2.c
 *
 * Shanmukh Iyer 2251372; Michael Imbert 2212593
 */
#include "challenges_part2.h"
#include <sys/sysinfo.h>

typedef struct {
    Matrix* A;
    Matrix* B;
    Matrix* result;
    int start_row;  // Start row for this thread to process
    int end_row;    // End row for this thread to process
} ThreadData;

// Function to multiply a row of A with all columns of B
// Function to multiply a range of rows in A with all columns of B
void* multiply_rows(void* arg) {
    ThreadData* data = (ThreadData*)arg;

    for (int i = data->start_row; i < data->end_row; i++) {
        for (int j = 0; j < data->B->cols; j++) {
            data->result->matrix[i][j] = 0;
            for (int k = 0; k < data->A->cols; k++) {
                data->result->matrix[i][j] += data->A->matrix[i][k] * data->B->matrix[k][j];
            }
        }
    }

    pthread_exit(NULL); // Terminate the thread
}

Matrix *multiply(Matrix *A, Matrix *B)
{
    printf("\n%d x %d & %d x %d;\n",A->rows, A->cols, B->rows, B->cols);
    // Check if matrix multiplication is possible
    if ((A->cols != B->rows) || 
    (A->cols == 0 || A->rows == 0 || B->cols == 0 || B->rows == 0))
    {
        perror("Size incompatibility");
        return NULL;
    }

    // Allocate memory for the result matrix
    Matrix* result = (Matrix*)malloc(sizeof(Matrix));
    result->rows = A->rows;
    result->cols = B->cols;

    // Allocate memory for the matrix elements
    result->matrix = (short**)malloc(result->rows * sizeof(short*));
    for (int i = 0; i < result->rows; i++) {
        result->matrix[i] = (short*)malloc(result->cols * sizeof(short));
    }

    // Get the number of available CPU cores
    int num_threads = get_nprocs();

    // Limit the number of threads to the number of rows in matrix A
    if (num_threads > A->rows) {
        num_threads = A->rows;
    }

    pthread_t threads[num_threads];
    ThreadData thread_data[num_threads];

    int rows_per_thread = A->rows / num_threads;
    int extra_rows = A->rows % num_threads;

    // Assign rows to threads
    int start_row = 0;
    for (int i = 0; i < num_threads; i++) {
        int end_row = start_row + rows_per_thread + (i < extra_rows ? 1 : 0);  // Distribute extra rows

        thread_data[i].A = A;
        thread_data[i].B = B;
        thread_data[i].result = result;
        thread_data[i].start_row = start_row;
        thread_data[i].end_row = end_row;

        pthread_create(&threads[i], NULL, multiply_rows, (void*)&thread_data[i]);

        start_row = end_row;
    }

    // Join threads
    for (int i = 0; i < num_threads; i++) {
        pthread_join(threads[i], NULL);
    }

    return result;
}

// This main is to help you in your development.
// Don't forget to comment it out when you run the tests.
// int main(int argc, char *argv[])
// {
//     // Example usage of the multiply function

//     // Create matrix A (2x3)
//     Matrix A;
//     A.rows = 2;
//     A.cols = 3;
//     A.matrix = (short **)malloc(A.rows * sizeof(short *));
//     A.matrix[0] = (short[]){1, 2, 3};
//     A.matrix[1] = (short[]){4, 5, 6};

//     // Create matrix B (3x2)
//     Matrix B;
//     B.rows = 3;
//     B.cols = 2;
//     B.matrix = (short **)malloc(B.rows * sizeof(short *));
//     B.matrix[0] = (short[]){7, 8};
//     B.matrix[1] = (short[]){9, 10};
//     B.matrix[2] = (short[]){11, 12};

//     // Multiply A and B
//     Matrix *result = multiply(&A, &B);

//     // Print result matrix
//     if (result != NULL)
//     {
//         printf("Result matrix:\n");
//         for (int i = 0; i < result->rows; i++)
//         {
//             for (int j = 0; j < result->cols; j++)
//             {
//                 printf("%d ", result->matrix[i][j]);
//             }
//             printf("\n");
//         }

//         // Free result matrix memory
//         for (int i = 0; i < result->rows; i++)
//         {
//             free(result->matrix[i]);
//         }
//         free(result->matrix);
//         free(result);
//     }

//     // Free A and B matrix memory
//     free(A.matrix);
//     free(B.matrix);

//     return 0;
// }
