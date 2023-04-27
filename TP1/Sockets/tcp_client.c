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
#define PORT 80

void kill_on_error(const char *fmt, ...);

void main(int argc, char **argv){

    int client_socket;
    struct sockaddr_in server_addr;
    char buffer[BUFFLEN];

    if (argc != 2) {
        kill_on_error("Comando: %s <server address>", argv[0]);
    }

    if ((client_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        kill_on_error("Erro ao criar o socket!");
    }
    printf("[+]Client Socket Created Sucessfully.\n");

    memset(&server_addr, '\0', sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = inet_addr(argv[1]);

    if (connect(client_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        kill_on_error("Erro! connect() falhou :(");
    }
    //connect(client_socket, (struct sockaddr*)&server_addr, sizeof(server_addr));
    printf("[+]Connected to Server.\n");

    recv(client_socket, buffer, BUFFLEN, 0);
    printf("[+]Data Recv: %s\n", buffer);
    printf("[+]Closing the connection.\n");

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