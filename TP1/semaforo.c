#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <stdbool.h>

#define NUMCONS 1
#define NUMPROD 2
#define BUFFERSIZE 1000

pthread_t cons[NUMCONS];
pthread_t prod[NUMPROD];
pthread_mutex_t mutex;
int buffer[BUFFERSIZE];
int currentidx;
int consumed = 0;
clock_t begin;
sem_t full, empty;

bool isPrime(int n)
{
    bool a = true;
    if (n == 0 || n == 1)
    {
        a = false;
    }
    else
    {
        for (int i = 2; i <= n / 2; ++i)
        {
            if (n % i == 0)
            {
                a = false;
                break;
            }
        }
    }
    return a;
}


void *produtor(void *arg)
{
    int n;
    while (1)
    {
        n = rand() % 10000000;

        sem_wait(&full);
        pthread_mutex_lock(&mutex);

        buffer[currentidx++] = n;

        pthread_mutex_unlock(&mutex);
        sem_post(&empty);

    }
}

void *consumidor(void *arg)
{
    int n;
    while (1)
    {
        sem_wait(&empty);
        pthread_mutex_lock(&mutex);

        n = buffer[--currentidx];

        pthread_mutex_unlock(&mutex);
        sem_post(&full);

        consumed++;
        if (isPrime(n))
        {
            printf("%d é primo \n", n);
        }
        else
        {
            printf("%d não é primo \n", n);
        }
        if (consumed == 100000){
            clock_t end = clock();
            double dur = (double)(end - begin) / CLOCKS_PER_SEC;
            printf("Foram utilizados %d produtores, %d consumidores, %d posições no buffer e a duração foi de %f segundos \n",NUMPROD, NUMCONS,BUFFERSIZE, dur);
            exit(0);
        }
    }
}

int main(int argc, char **argv)
{
    int i;
    begin = clock();
    pthread_mutex_init(&mutex, NULL);
    sem_init(&full, 0, BUFFERSIZE);
    sem_init(&empty, 0, 0);

    for (i = 0; i < NUMCONS; i++)
        pthread_create(&(cons[i]), NULL, consumidor, NULL);

    for (i = 0; i < NUMPROD; i++)
        pthread_create(&(prod[i]), NULL, produtor, NULL);

    for (i = 0; i < NUMCONS; i++)
        pthread_join(cons[i], NULL);

    for (i = 0; i < NUMPROD; i++)
        pthread_join(prod[i], NULL);
}
