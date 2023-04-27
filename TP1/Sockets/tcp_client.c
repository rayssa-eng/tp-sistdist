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

#define BUFFLEN 1024
#define PORT 8080
int N = 1;
int i = 0;
char res[BUFFLEN];
void kill_on_error(const char *fmt, ...);

int produtor(int client_socket,int max) { // produz número, envia, espera resultado, imprime
    while (i < max + 1) {
        int delta = rand() % 100;
        printf("vou enviar\n");
        //Converter N para string e enviar string
        //send(client_socket, &N, BUFFLEN, 0);
        send(client_socket, "Teste", BUFFLEN, 0);
        N = N + delta;
        printf("aguardando resposta\n");
        //recv(client_socket, &res, BUFFLEN, MSG_WAITALL);
        //printf("%s", res);
        i++;
    }
    N = 0;
    send(client_socket, &N, BUFFLEN, 0);
}

void main(){

    int client_socket;
    struct sockaddr_in server_addr;
    char buffer[BUFFLEN];

    if ((client_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        kill_on_error("Erro ao criar o socket!");
    }
    printf("[+]Client Socket Created Sucessfully.\n");

    memset(&server_addr, '\0', sizeof(server_addr));

    bzero(&server_addr, sizeof(server_addr));

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    if (connect(client_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        kill_on_error("Erro! connect() falhou :(");
    }
    //connect(client_socket, (struct sockaddr*)&server_addr, sizeof(server_addr));
    printf("[+]Connected to Server.\n");

    int max = rand() % 1000;
    produtor(client_socket, max);
}


void kill_on_error(const char *fmt, ...) {
    int errno_save;
    va_list ap;

    //salvando errno pois qualquer chamada (para biblioteca ou sistema operacional) pode atribuir à essa var
    errno_save = errno;

    //print de fmt+args pra stdout
    va_start(ap, fmt);
    vfprintf(stdout,fmt,ap);
    fprintf(stdout, "\n");
    fflush(stdout);

    //print mensagem de erro de acordo com o valor setado em err_no primeiramente
    if (errno_save != 0) {
        fprintf(stdout, "errno = %d : %s\n", errno_save, strerror(errno_save));
        fprintf(stdout, "\n");
        fflush(stdout);
    }
    va_end(ap);

    //aqui a gente mata
    exit(1);
}