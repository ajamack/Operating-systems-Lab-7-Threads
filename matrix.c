#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

#define MAX 20
#define NUM_THREADS 10

int matA[MAX][MAX]; 
int matB[MAX][MAX]; 

int matSumResult[MAX][MAX];
int matDiffResult[MAX][MAX]; 
int matProductResult[MAX][MAX]; 

typedef struct {
  int start;
  int end;
} Range;

void fillMatrix(int matrix[MAX][MAX]) {
    for(int i = 0; i < MAX; i++) {
        for(int j = 0; j < MAX; j++) {
            matrix[i][j] = rand() % 10 + 1;
        }
    }
}

void printMatrix(int matrix[MAX][MAX]) {
    for(int i = 0; i < MAX; i++) {
        for(int j = 0; j < MAX; j++) {
            printf("%5d", matrix[i][j]);
        }
        printf("\n");
    }
    printf("\n");
}

// Fetches the appropriate coordinates from the argument, and sets
// the cell of matSumResult at the coordinates to the sum of the
// values at the coordinates of matA and matB.
void* computeSum(void* args) {
    Range *r = (Range *)args;
    for (int idx = r->start; idx < r->end; idx++) {
        int i = idx / MAX;
        int j = idx % MAX;
        matSumResult[i][j] = matA[i][j] + matB[i][j];
    }
    return NULL; // pass in the number of the ith thread
}

// Fetches the appropriate coordinates from the argument, and sets
// the cell of matDiffResult at the coordinates to the difference of the
// values at the coordinates of matA and matB.
void* computeDiff(void* args) {
    Range *r = (Range *)args;
    for (int idx = r->start; idx < r->end; idx++) {
        int i = idx / MAX;
        int j = idx % MAX;
        matDiffResult[i][j] = matA[i][j] - matB[i][j];
    }
    return NULL;
}

// Fetches the appropriate coordinates from the argument, and sets
// the cell of matProductResult at the coordinates to the inner product
// of matA and matB.
void* computeProduct(void* args) {
    Range *r = (Range *)args;
    for (int idx = r->start; idx < r->end; idx++) {
        int i = idx / MAX;
        int j = idx % MAX;

        int sum = 0;
        for (int k = 0; k < MAX; k++) {
            sum += matA[i][k] * matB[k][j];
        }
        matProductResult[i][j] = sum;
    }
    return NULL;
}

// One worker thread does all three operations (sum, diff, product)
// over its assigned range of cells.
void* worker(void* args) {
    Range *r = (Range *)args;

    computeSum(r);
    computeDiff(r);
    computeProduct(r);

    free(r);   // free the Range once all three are done
    return NULL;
}

// Spawn a thread to fill each cell in each result matrix.
// How many threads will you spawn?
int main() {
    srand(time(0));
    
    fillMatrix(matA);
    fillMatrix(matB);  // Do Not Remove. Just ignore and continue below.
    
    // 0. Get the matrix size from the command line and assign it to MAX
    
    // 1. Fill the matrices (matA and matB) with random values.
    
    // 2. Print the initial matrices.
    printf("Matrix A:\n");
    printMatrix(matA);
    printf("Matrix B:\n");
    printMatrix(matB);
    
    // 3. Create pthread_t objects for our threads.
    pthread_t threads[NUM_THREADS];
    int totalCells = MAX * MAX;          
    int chunkSize  = totalCells / NUM_THREADS;

    // 4. Create a thread for each cell of each matrix operation.
    //    (Here, each of the 10 threads handles a chunk of cells
    //     and computes sum, diff, and product for its chunk.)
    for (int t = 0; t < NUM_THREADS; t++) {
        Range *r = malloc(sizeof(Range));
        if (!r) {
            perror("malloc");
            exit(1);
        }
        r->start = t * chunkSize;
        r->end   = (t == NUM_THREADS - 1) ? totalCells : (t + 1) * chunkSize;

        pthread_create(&threads[t], NULL, worker, r);
    }
    
    // 5. Wait for all threads to finish.
    for (int t = 0; t < NUM_THREADS; t++) {
        pthread_join(threads[t], NULL);
    }

    // 6. Print the results.
    printf("Results:\n");
    printf("Sum:\n");
    printMatrix(matSumResult);
    printf("Difference:\n");
    printMatrix(matDiffResult);
    printf("Product:\n");
    printMatrix(matProductResult);
    return 0;
}
