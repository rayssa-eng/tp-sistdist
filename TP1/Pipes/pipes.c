#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <stdbool.h>
#include <sys/types.h>
#include <signal.h>

int N = 1;
int i = 0;
pid_t pid;
int pipeend[2];
int mypipe;
int myread;

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

int produtor(int max)
{
    close(pipeend[0]);
    while (i < max)
    {
        int delta = rand() % 100;
        write(pipeend[1], &N, sizeof(N));
        N = N + delta;
        i++;
    }
    N = 0;
    write(pipeend[1], &N, sizeof(N));
   // exit(0);

}

int j = 0;
int consumidor()
{
    close(pipeend[1]);
    read(pipeend[0], &myread, sizeof(myread));

    while (myread != 0)
    {
        if (isPrime(myread))
        {
            printf("%d é primo \n", myread);
        }
        else
        {
            printf("%d não é primo \n", myread);
        }
        read(pipeend[0], &myread, sizeof(myread));
    }
    printf("Fim! Ja veio o %d.", myread);
    exit(1);
}

int main()
{
    srand(time(NULL));
    mypipe = pipe(pipeend);
    pid = fork();
    int max = rand() % 1000;
    if (pid < 0) {
        printf("Erro ao criar novo processo\n");
    }
    else if (pid > 0) {
        if (mypipe < 0)
        {
            printf("Erro ao criar pipe\n");
        }
        produtor(max);
    }
    else
    {
        consumidor();
    }
}