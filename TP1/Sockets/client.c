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

//standard HTTP Port
#define SERVER_PORT 80

#define MAXLINE 4096 //buffer length where data goes
#define SA struct sockaddr

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

int main(int argc, char **argv) {
    int sockfd, n;
    int sendbytes;
    struct sockaddr_in servaddr;
    char sendline[MAXLINE];
    char recvline[MAXLINE];

    if (argc != 2) {
        kill_on_error("Comando: %s <server address>", argv[0]);
    }

    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        kill_on_error("Erro ao criar o socket!");
    }
}