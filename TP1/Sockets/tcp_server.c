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

#define PORT 80
#define BUFFLEN 1024

void kill_on_error(const char *fmt, ...);

void main(int argc, char **argv){
    if (argc != 2) {
        kill_on_error("Comando: %s <server address>", argv[0]);
    }

    int sockfd;
    struct sockaddr_in server_addr;

    int new_socket;
    struct sockaddr_in newAddr;

    socklen_t addr_size;
    char buffer[BUFFLEN];

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    printf("[+]Server Socket Created Sucessfully.\n");
    memset(&server_addr, '\0', sizeof(server_addr));

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = inet_addr(argv[1]);

    if ((bind(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr))) < 0) { // Víncula socket ao servidor
        kill_on_error("Erro no vínculo\n");
    }

    printf("[+]Bind to Port number %d.\n", PORT);

    listen(sockfd, 5);
    printf("[+]Listening...\n");

    new_socket = accept(sockfd, (struct sockaddr*)&newAddr, &addr_size);

    strcpy(buffer, "Hello");
    send(new_socket, buffer, strlen(buffer), 0);
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