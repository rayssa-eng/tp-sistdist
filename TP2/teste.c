#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

#define VECTOR_SIZE 15
#define NUM_THREADS 3

int *vector;// = {1, 2, 3, 4, /* ... */}; // Input vector
int partialSums[NUM_THREADS] = {0}; // Array to store partial sums

typedef struct {
    int lock;
} Spinlock;

void acquire_spinlock(Spinlock* spinlock) {
    while (__sync_lock_test_and_set(&spinlock->lock, 1) == 1);
}

void release_spinlock(Spinlock* spinlock) {
    __sync_lock_release(&spinlock->lock);
}

void* sum_vector_elements(void* arg) {
    Spinlock* spinlock = (Spinlock*)arg;
    int thread_id = pthread_self() % NUM_THREADS;
    int start = thread_id * (VECTOR_SIZE / NUM_THREADS);
    int end = start + (VECTOR_SIZE / NUM_THREADS);
    
    acquire_spinlock(spinlock);
    
    int partial_sum = 0;
    for (int i = start; i < end; i++) {
        partial_sum += vector[i];
    }
    
    partialSums[thread_id] = partial_sum;
    
    release_spinlock(spinlock);
    
    return NULL;
}

int main() {
    srand(time(NULL));
    vector = (int *)malloc(VECTOR_SIZE * (sizeof(int)));
    
    // initialize vector x with some values
    for (int i = 0; i < VECTOR_SIZE; i++) {
        int rand_num = (int)rand() % 201 - 100;
        vector[i] = rand_num;
    }
    
    pthread_t threads[NUM_THREADS];
    Spinlock spinlock;
    int total_sum = 0;
    
    // Initialize the spinlock
    spinlock.lock = 0;
    
    // Create threads
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_create(&threads[i], NULL, sum_vector_elements, &spinlock);
    }
    
    // Wait for threads to complete
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }
    
    // Calculate the total sum
    for (int i = 0; i < NUM_THREADS; i++) {
        printf(" %d ", partialSums[i]);
        total_sum += partialSums[i];
    }

    printf("\n\nv = [\n");
    for(int i = 0; i < VECTOR_SIZE; i++) {
        printf(" %d ", vector[i]);
    }
    printf("]\n");

    printf("Total Sum: %d\n", total_sum);
    
    return 0;
}
