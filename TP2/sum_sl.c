#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#include <stdatomic.h>
#include <stdbool.h>

#define K 4 //num de threads
#define N 10 //tamanho do vetor

int v[N];
int parts_of_sum[K]; //somas de cada parcela K

typedef struct { //spinlock
    bool held;
} spinlock;

bool test_and_set(bool *flag) {
    bool prev = *flag;
    *flag = true;
    return prev;
}

void acquire(spinlock* lock) {
    while(test_and_set(&lock->held)); //busy wait c test_and_set
}

void release(spinlock* lock) {
    lock->held = 0;
}

void* acumulador(void *arg) {
    spinlock* lock = (spinlock*)arg; 

    int thread_id = pthread_self() % K;
    
    int start_thread = thread_id * (N / K);
    int end_thread = start_thread + (N / K);

    int partial_sum = 0;
    for (int k = start_thread; k < end_thread; k++) {
        partial_sum += v[k];
    }
    
    acquire(lock);
    parts_of_sum[thread_id] = partial_sum;
    release(lock);
    
    return NULL;

}

int* create_v(int n) {
    srand(time(NULL));
    
    int *x;              // vector x
    x = (int *)malloc(n * (sizeof(int)) / 2);
    // initialize vector x with some values
    for (int i = 0; i < n; i++) {
        int rand_num = (int)rand() % 201 - 100;
        x[i] = rand_num;
    }
    return x;
}

int main() {


    printf("[");
    for (int j = 0; j < N; j++) {
        printf("%f,", x[j]);
    }
    printf("]");

    return 0;
}