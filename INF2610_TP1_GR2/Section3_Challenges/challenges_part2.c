/*
 * Ecole Polytechnique Montreal - GIGL
 * Automne 2024
 * Challenges - part2.c
 *
 * Shanmukh Iyer 2251372; Michael Imbert 2212593
*/
#include "challenges_part2.h"

Matrix* multiply(Matrix* A, Matrix* B) {
    // Check if matrix multiplication is possible
    if (A->cols != B->rows) {
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

    // Perform matrix multiplication
    for (int i = 0; i < A->rows; i++) {
        for (int j = 0; j < B->cols; j++) {
            result->matrix[i][j] = 0; // Initialize to zero
            for (int k = 0; k < A->cols; k++) {
                result->matrix[i][j] += A->matrix[i][k] * B->matrix[k][j];
            }
        }
    }

    return result;
}


// This main is to help you in your development. 
// Don't forget to comment it out when you run the tests.
// int main(int argc, char*argv[])
// {
   
//     return 0;
// }
