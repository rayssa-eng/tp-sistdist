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
#include <errno.h>

int N = 1;
int i = 0;
pid_t pid;
int myread;

int sockid;
int connection, len;
struct sockaddr_in server, client;

#define PORT 8000
#define size 20

char ans[size];
char res[size];

bool isPrime(int n) {
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

int produtor(int max) { // produz número, envia, espera resultado, imprime
    while (i < max + 1) {
        int delta = rand() % 100;
        N = N + delta;
        send(sockid, &N, sizeof(N), MSG_CONFIRM);
        recv(sockid, &res, size, MSG_WAITALL);
        printf("%s", res);
        i++;
    }
    N = 0;
    send(sockid, &N, sizeof(N), MSG_CONFIRM);
}

int j = 0;
int consumidor() { // recebe numero, testa se é primo, devolve resposta
    recv(sockid, &myread, sizeof(myread), MSG_WAITALL);

    while (myread != 0)
    {
        if (isPrime(myread)) {
            //strcpy(ans, ("%d é primo \n", myread));
        } else {
            //strcpy(ans, ("%d não é primo \n", myread));
        }
        send(sockid, &ans, size, MSG_CONFIRM);
        recv(sockid, &myread, sizeof(myread), MSG_WAITALL);
    }
}

int main() {
    srand(time(NULL));

    // Criação do socket

    sockid = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP); // AF_INET - adress family que suporta ipv4, formato ip+porta
                                                                            //  SOCK_STREAM - tipo de socket com maior confiabilidade dos dados
    if (sockid < 0) {                                                           // IPPROTO_TCP - protocolo TCP de comunicação da internet
        printf("Erro ao criar socket\n");
    }

    pid = fork();
    int max = rand() % 1000;

    if (pid < 0) {
        printf("Erro ao criar novo processo\n");
    } else if (pid > 0) {
        printf("P1. Eu sou o pai, meu pid é %d\n", getpid());
        server.sin_family = AF_INET;
        server.sin_addr.s_addr = htonl(INADDR_ANY);
        server.sin_port = htons(PORT);
        // htonl e htons convertem o valor do endereço e da porta para o formato da rede
        // INADDR_ANY faz com que o socket se conecte a todas as interfaces locais disponíveis

        if ((bind(sockid, (struct sockaddr *)&server, sizeof(server))) < 0) { // Víncula socket ao servidor
            printf("Erro %d no vínculo\n", errno);
        } else {
            listen(sockid, 3); // Socket espera uma conexão
            // consumidor();
        }
    } else {
        printf("P2. Eu sou o filho, meu pid é %d\n", getpid());
        len = sizeof(client);
        connection = accept(sockid, (struct sockaddr *)&client, (socklen_t *)&len); // Tenta conectar cliente ao servidor

        //printf("%d\n", connection);

        if (connection < 0) {
            printf("Erro %d ao aceitar cliente\n", errno);
        } else {
            printf("Conexão realizada\n");
            // produtor(max);
        }
    }
}