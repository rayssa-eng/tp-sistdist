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
#define size 20
char in[size];
char w[size] = "Teste";
char h[size] = "Teste2";
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
    while (i < max + 1)
    {
        int delta = rand() % 100;
        N = N + delta;
        //w = N + '0';
        write(pipeend[1], w, sizeof(w));
        i++;

    }
    N = 0;
    //w = N + '0';
    write(pipeend[1], w, sizeof(w));
    printf("mandou zero");
}

int j = 0;
int consumidor()
{
    close(pipeend[1]);
    printf("Começou leitura e char é %s \n", in);
    read(pipeend[0], in, sizeof(in));
    printf("Primeiro char é %s \n",in);

    //int r = in - '0';
    
    read(pipeend[0], in, sizeof(in));
    //r = in - '0';
    printf("Segundo char é %s \n",in);
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
    mypipe = pipe(pipeend);
    pid = fork();
    int max = rand() % 1000;
    if (pid < 0)
    {
        printf("Erro ao criar novo processo\n");
    }
    else if (pid > 0)
    {
        printf("processo 1\n");
        if (mypipe < 0)
        {
            printf("Erro ao criar pipe\n");
        }

        produtor(max);
    }
    else
    {
        printf("processo 2\n");
        consumidor();
    }
}