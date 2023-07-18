#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/time.h>
#include <time.h>
#include <stddef.h>
#define MAX_BUFFER_SIZE 1024

#define PORT 8080
#define MAXLINE 1024
#define SIZE 10
#define r 10
#define k 4

int sockfd, connfd;
int myId;
char id[10];
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
    char buffer[MAXLINE];
    const char *request = "1";
    const char *grant = "2";
    char request_id[SIZE];
    char grant_id[SIZE];
    strcpy(request_id, request);
    strcpy(grant_id, grant);
    strcat(request_id, id);
    strcat(grant_id, id);
    send(sockfd, request_id, strlen(request_id), 0);
    printf(" \n Mensagem enviada request: %s\n", request_id);

    socklen_t len = sizeof(servaddr); // Correção adicionada aqui
    int msg;
    //msg = recvfrom(sockfd, buffer, MAXLINE, MSG_WAITALL, (struct sockaddr *)&servaddr, &len);
    msg = recv(sockfd, buffer,MAX_BUFFER_SIZE,0);
    buffer[msg] = '\0';
    //printf("buf: %s, gid = %s \n", buffer, grant_id);
    if (strcmp(buffer, grant_id) == 0) {
        return 1;
    } else {
        return -1;
    }
}

void my_run() {
    int i = 0;
    FILE *myFile;
    myId = getpid();
    sprintf(id, "%d", myId);
    const char *release = "3";
    char release_id[SIZE];
    strcpy(release_id, release);
    strcat(release_id, id);

    socklen_t len = sizeof(servaddr);
    while (i < r) {
        int access = get_access();
        if (access == 1) {
            myFile = fopen("resultado.txt", "a");
            time_t current_time;
            struct timeval tv;
            char *c_time_string;
            char formatted_time[100];  // Buffer to store formatted time
            current_time = time(NULL);
            gettimeofday(&tv, NULL);
            c_time_string = ctime(&current_time);

            // Format the time string with milliseconds and year
            strftime(formatted_time, sizeof(formatted_time), "%a %b %d %H:%M:%S:", localtime(&current_time));
            sprintf(formatted_time + strlen(formatted_time), "%03ld %04d\n", tv.tv_usec / 1000, localtime(&current_time)->tm_year + 1900);  // Append milliseconds and year

            fprintf(myFile, "%s %s", id, formatted_time);
            fclose(myFile);

            sleep(k);
            send(sockfd, release_id, strlen(release_id), 0);
            printf("\n Mensagem enviada release: %s\n", release_id);
            i++;
        }
    }
}
