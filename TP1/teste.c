#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <stdbool.h>
#include <sys/types.h>
#include <netdb.h>
#include <netinet/in.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>


int N = 1;
int i = 0;
pid_t pid;
int myread;

int sockid;
int connection, len;
struct sockaddr_in server, client;
#define MAX 80
#define PORT 8080

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
        write(sockid, &N, sizeof(N));
        i++;
    }
    N = 0;
    write(sockid, &N, sizeof(N));


}

int j = 0;
int consumidor()
{
    read(sockid, &myread, sizeof(myread));

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
        read(sockid, &myread, sizeof(myread));

    }
}

int main()
{
    srand(time(NULL));
    pid = fork();
    int max = rand() % 1000;
    if (pid < 0)
    {
        printf("Erro ao criar novo processo\n");
    }
    else if (pid > 0)
    {   

        //Criação do socket
        sockid = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP); // AF_INET - adress family que suporta ipv4, formato ip+porta
                                            //  SOCK_STREAM - tipo de socket com maior confiabilidade dos dados
                                            // IPPROTO_TCP - protocolo TCP de comunicação da internet
    
        if (sockid < 0)
        {
            printf("Erro ao criar socket\n");
        }

        server.sin_family = AF_INET;
        server.sin_addr.s_addr = htonl(INADDR_ANY);
        server.sin_port = htons(PORT);
        //htonl e htons convertem o valor do endereço e da porta para o formato da rede
        //INADDR_ANY faz com que o socket se conecte a todas as interfaces locais disponíveis
        
        if ((bind(sockid, (struct sockaddr *)&server , sizeof(server))) != 0) //Víncula socket ao servidor
        {
            printf("Erro no vínculo\n");
        }

        listen(sockid , 3); //Socket espera uma conexão

        produtor(max);
    }
    else
    {
        len = sizeof(client);    

        connection = accept(sockid, (struct sockaddr *)&client, (socklen_t*)&len); //Tenta conectar cliente ao servidor

        if (connection < 0)
        {
            printf("Erro ao aceitar cliente\n");
        }
        consumidor();
    }
}