#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <stdbool.h>
#include <sys/types.h>
int N = 1;
int i = 0;
pid_t pid;
int pipeend[2];
int mypipe;
int size = 20;

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
    while (i < max + 1)
    {
        int delta = rand() % 100;
        N = N + delta;
        char w = N + '0';
        //printf("%d num1 \n e char %c", N,w);

        write(pipeend[1], &w, size);
        i++;

    }
    N = 0;
    char w = N + '0';
    write(pipeend[1], &w, size);
    printf("mandou zero");
}

int j = 0;
int consumidor()
{
    char in;
    read(pipeend[0], in, size);
    int r = in - '0';
    printf(in);
    read(pipeend[0], in, size);
    r = in - '0';
    printf(in);
    //while (r != 0)
    //{
        //printf("%d atual \n", r);
        //if (isPrime(r))
        //{
        //    printf("%d é primo \n", r);
        //}
        //read(pipeend[0], in, size);
    //}
    printf("terminou");
}

int main()
{
    srand(time(NULL));
    pid = fork();
    //printf("meu pid é %d\n", pid);
    int max = rand() % 1000;
    mypipe = pipe(pipeend);
    if (pid < 0)
    {
        printf("Erro ao criar novo processo\n");
    }
    else if (pid > 0)
    {
        //printf("processo 1\n");
        if (mypipe < 0)
        {
            printf("Erro ao criar pipe\n");
        }

        produtor(max);
    }
    else
    {
        //printf("processo 2\n");
        consumidor();
    }
}