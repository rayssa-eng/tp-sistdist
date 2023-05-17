#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#include <stdbool.h>
#include <errno.h>

#define K 256 // número de threads
#define N 1000000000 // tamanho do vetor
#define MAX 100 // numero aleatorio maximo
#define MIN -100 // numero aleatorio minimo

int power = 0;
int *v;
int parts_of_sum[K] = {0}; // somas de cada parte
int total_sum = 0;
FILE *file;

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
    // inicializando v com valores aleatorios entre [-100,100]
    for (int i = 0; i < n; i++) {
        int rand_num = (int)rand() % (MAX - (MIN) + 1) + MIN;
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
    char fileName[50];

    // formatando o nome do arquivo a ser gerado
    sprintf(fileName, "n_9_k_%d.csv", K);

    file = fopen(fileName, "w"); // Open the file in write mode ("w")
    int x = 0;

    clock_t start, end;    // variables to store CPU time
    double cpu_time_used;  // variable to store elapsed CPU time

    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC; // compute elapsed CPU time
    
    pthread_t threads[K];
    pthread_t total_thread;

    // cada thread com um spinlock
    int thread_ids[K];
    int total_sum_k = 0;

    while (x < 10) {
        
        v = create_v(N);

        // printf("v = [ ");
        // for (int j = 0; j < N; j++) {
        //     printf("%d ", v[j]);
        // }
        // printf("]\n\n");

        start = clock(); // começa a contar o tempo
        // criar threads
        for (int i = 0; i < K; i++) {
            thread_ids[i] = i;
            pthread_create(&threads[i], NULL, accumulator, (void *)&thread_ids[i]); // Pass the address of thread_ids[i] as the argument
        }

        // espera uma thread retornar
        for (int i = 0; i < K; i++) {
            pthread_join(threads[i], NULL);
        }
        end = clock(); // fim da contagem de tempo

        cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;

        pthread_create(&total_thread, NULL, adder, NULL); // criando a thread que calcula o valor completo da soma
        pthread_join(total_thread, NULL);

        // calcula a soma total a partir de um pedaço de soma em cada thread
        for (int i = 0; i < K; i++) {
            printf("Soma parcial executada pela thread %d: %d\n", i, parts_of_sum[i]);
            total_sum_k += parts_of_sum[i];
        }

        printf("\nSoma total por uma thread separada das outras: %d\n", total_sum);
        printf("Soma total calculada pela somas de cada parte: %d\n", total_sum_k);

        free(v);

        if ((check_equality(total_sum, total_sum_k))) {} else { exit(-1); }


        if (file == NULL) {
            printf("Failed to open the file.\n");
            return 1;
        }

        fprintf(file, "%d, %f\n", x, cpu_time_used);
        x++;
                
    }
    fclose(file); // Close the file
    return 0;
}
