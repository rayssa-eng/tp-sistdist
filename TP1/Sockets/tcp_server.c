#include <sys/ioctl.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <signal.h>
#include <stdio.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <stdarg.h>
#include <errno.h>
#include <fcntl.h>
#include <netdb.h>
#include <stdbool.h>

#define PORT 8000
#define BUFFLEN 1024

char *myread[BUFFLEN];
char ans[BUFFLEN];

void kill_on_error(const char *fmt, ...);

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

int j = 0;
int consumidor(int new_socket)
{ // recebe numero, testa se é primo, devolve resposta
    printf("Aguardando\n");
    int num = 0;
    recv(new_socket, &myread, BUFFLEN, 0);
    int myread_int = atoi(myread);

    while (myread_int != 0)
    {
        if (isPrime(myread_int))
        {
            strcpy(ans, "é primo \n");
        }
        else
        {
            strcpy(ans, "não é primo \n");
        }
        send(new_socket, ans, BUFFLEN, 0);
        recv(new_socket, &myread, BUFFLEN, 0);

        myread_int = atoi(myread);
    }
    exit(0);
}

void main()
{
    int sockfd;
    struct sockaddr_in server_addr;

    int new_socket;
    struct sockaddr_in newAddr;

    socklen_t addr_size;
    char buffer[BUFFLEN];

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    printf("[+]Server Socket Created Sucessfully.\n");
    memset(&server_addr, '\0', sizeof(server_addr));

    bzero(&server_addr, sizeof(server_addr));

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    if ((bind(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr))) < 0)
    { // Víncula socket ao servidor
        kill_on_error("Erro no vínculo\n");
    }

    printf("[+]Bind to Port number %d.\n", PORT);

    listen(sockfd, 5);
    printf("[+]Listening...\n");

    new_socket = accept(sockfd, (struct sockaddr *)&newAddr, &addr_size);

    consumidor(new_socket);
}

void kill_on_error(const char *fmt, ...)
{
    int errno_save;
    va_list ap;

    // salvando errno pois qualquer chamada (para biblioteca ou sistema operacional) pode atribuir à essa var
    errno_save = errno;

    // print de fmt+args pra stdout
    va_start(ap, fmt);
    vfprintf(stdout, fmt, ap);
    fprintf(stdout, "\n");
    fflush(stdout);

    // print mensagem de erro de acordo com o valor setado em err_no primeiramente
    if (errno_save != 0)
    {
        fprintf(stdout, "errno = %d : %s\n", errno_save, strerror(errno_save));
        fprintf(stdout, "\n");
        fflush(stdout);
    }
    va_end(ap);

    // aqui a gente mata
    exit(1);
}