#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#include <stdbool.h>
#include <errno.h>

#define K 5 // número de threads
#define N 15 // tamanho do vetor
#define MAX 100 // numero aleatorio maximo
#define MIN -100 // numero aleatorio minimo

int *v;
int parts_of_sum[K] = {0}; // somas de cada parte
int total_sum = 0;

typedef struct {
    bool held;
} spinlock;

bool test_and_set(bool *flag) {
    bool prev = *flag;
    *flag = true;
    return prev;
}

void acquire(spinlock* lock) {
    while (test_and_set(&lock->held));
}

void release(spinlock* lock) {
    lock->held = false;
}

void* accumulator(void *arg) {
    spinlock locks[K];
    int thread_id = *(int*)arg; // thread ID é o argumento direto

    int start_thread = thread_id * (N / K);
    int end_thread = start_thread + (N / K);

    int partial_sum = 0;
    for (int k = start_thread; k < end_thread; k++) {
        partial_sum += v[k];
    }

    acquire(&locks[thread_id]);
    parts_of_sum[thread_id] = partial_sum;
    release(&locks[thread_id]);

    return NULL;
}

void* adder(void* arg) {
    for (int i = 0; i < N; i++) {
        total_sum += v[i];
    }
    return NULL;
}

int* create_v(int n) {
    srand(time(NULL));

    // vetor v
    v = (int *)malloc(n * sizeof(int));
    // inicializando
    for (int i = 0; i < n; i++) {
        int rand_num = (int)rand() % (100 - (-100) + 1) + -100;
        v[i] = rand_num;
    }
    return v;
}

int check_equality(int a, int b) {
    if (a == b) {
        printf("\nOs valores coincidem!\n");
        return 1;
    } else {
        printf("\nOs valores não coincidem... Saindo...\n");
        return 0;
    }
}

int main() {
    file = fopen("teste.csv", "w");

    pthread_t threads[K];
    pthread_t total_thread;
     // Separate spinlock for each thread
    int thread_ids[K];
    int total_sum_k = 0;
    //int total_sum = 0;
    int x = 0;
    while (x < 10) {

        v = create_v(N);

        printf("v = [ ");
        for (int j = 0; j < N; j++) {
            printf("%d ", v[j]);
        }
        printf("]\n\n");

        pthread_create(&total_thread, NULL, adder, NULL);
        pthread_join(total_thread, NULL);

        // Create threads
        for (int i = 0; i < K; i++) {
            thread_ids[i] = i;
            pthread_create(&threads[i], NULL, accumulator, (void *)&thread_ids[i]); // Pass the address of thread_ids[i] as the argument
        }

        // Wait for threads to complete
        for (int i = 0; i < K; i++) {
            pthread_join(threads[i], NULL);
        }

        // Calculate the total sum
        for (int i = 0; i < K; i++) {
            printf("Soma parcial executada pela thread %d: %d\n", i, parts_of_sum[i]);
            total_sum_k += parts_of_sum[i];
        }

        printf("\nSoma total por uma thread separada das outras: %d\n", total_sum);
        printf("Soma total calculada pela somas de cada parte: %d\n", total_sum_k);

        free(v);

        if ((check_equality(total_sum, total_sum_k))) {

        } else {
            exit(-1);
        }
        x++;
    }
    return 0;
}
