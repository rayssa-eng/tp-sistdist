#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <time.h>
#include <errno.h>
#include <stddef.h>
#include <stdbool.h>

#define PORT 8080
#define MAXLINE 1024
#define SIZE 10
#define r 10
#define k 4

int sockfd, connfd;
int myId;
char id[4];
struct sockaddr_in servaddr, cli;

void my_run();
int get_access();

int main() {
    myId = getpid();
    sprintf(id, "%d", myId);
    const char *request = "1";
    const char *grant = "2";
    const char *release = "3";
    char request_id[SIZE];
    char grant_id[SIZE];
    char release_id[SIZE];
    strcpy(request_id, request);
    strcpy(grant_id, grant);
    strcpy(release_id, release);

    strcat(request_id, id);
    strcat(grant_id, id);
    strcat(release_id, id);
    
    printf("id: %s, request: %s, grant: %s, release: %s \n",id,request_id,grant_id,release_id);

    socklen_t len = sizeof(servaddr);
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }
    
    bzero(&servaddr, sizeof(servaddr)); 
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    servaddr.sin_port = htons(PORT);
    
     if (connect(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) == -1) {
        perror("connection with the server failed...\n");
        exit(EXIT_FAILURE);
    }
    
    my_run();
    
    return 0;
}

int get_access() {
    printf("getacess \n");
    char buffer[MAXLINE];
    const char *request = "1";
    const char *grant = "2";
    char request_id[SIZE];
    char grant_id[SIZE];
    strcpy(request_id, request);
    strcpy(grant_id, grant);
    strcat(request_id, id);
    strcat(grant_id, id);
    printf("premsg \n");
    sendto(sockfd, request_id, strlen(request_id), MSG_CONFIRM, (const struct sockaddr *)&servaddr, sizeof(servaddr));
    socklen_t len = sizeof(servaddr); // Correção adicionada aqui
    int msg;
    printf("prerecv \n");
    msg = recvfrom(sockfd, buffer, MAXLINE, MSG_WAITALL, (struct sockaddr *)&servaddr, &len);
    buffer[msg] = '\0';
    printf("buf: %s, gid = %s \n", buffer, grant_id);
    if (strcmp(buffer, grant_id) == 0) {
        return 1;
    } else {
        return -1;
    }
}

void my_run() {
    int i = 0;
    FILE *myFile;
    char release_id[SIZE]; // Declaração adicionada aqui
    strcpy(release_id, "3");
    strcat(release_id, id);   

    socklen_t len = sizeof(servaddr);
    while (i < r) {
        printf("i = %d \n", i);
        int access = get_access();
        if (access == 1) {
            myFile = fopen("resultado.txt", "a");
            time_t current_time;
            char *c_time_string;
            current_time = time(NULL);
            c_time_string = ctime(&current_time);
            fprintf(myFile, "%s %s", id, c_time_string);
            fclose(myFile);

            sleep(k);
            sendto(sockfd, release_id, strlen(release_id), MSG_CONFIRM, (const struct sockaddr *)&servaddr, sizeof(servaddr));
            i++;
            printf("i = %d, r = %d \n", i,r);
        }
    }
}
