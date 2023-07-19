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
double get_elapsed_time(const char* filename);


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
    
<<<<<<< HEAD:TP3/try/client.c
    
=======
>>>>>>> 73d981f66a014bdc749ea5ab0d24e2502bff65e2:TP3/client.c
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
    
    double elapsed_time = get_elapsed_time("resultado.txt");
    printf("Elapsed time: %.2f seconds\n", elapsed_time);

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

double get_elapsed_time(const char* filename) {
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }

    char line[MAXLINE];
    char first_timestamp[MAXLINE];
    char last_timestamp[MAXLINE];
    int line_count = 0;

    while (fgets(line, sizeof(line), file) != NULL) {
        line_count++;
        if (line_count == 1) {
            strcpy(first_timestamp, line);
        }
        strcpy(last_timestamp, line);
    }

    fclose(file);

    // Extract the timestamps from the lines
    char* first_timestamp_value = strchr(first_timestamp, ' ') + 1;
    char* last_timestamp_value = strchr(last_timestamp, ' ') + 1;

    struct tm tm_first;
    struct tm tm_last;
    strptime(first_timestamp_value, "%a %b %d %H:%M:%S:", &tm_first);
    strptime(last_timestamp_value, "%a %b %d %H:%M:%S:", &tm_last);
    time_t time_first = mktime(&tm_first);
    time_t time_last = mktime(&tm_last);

    double elapsed_time = difftime(time_last, time_first);
    return elapsed_time;
}

void my_run() {
    int i = 0;
    FILE *myFile;
<<<<<<< HEAD:TP3/try/client.c
    char release_id[SIZE]; // Declaração adicionada aqui
    strcpy(release_id, "3");
    strcat(release_id, id);  

    // Record the start time
    time_t start_time;
    time(&start_time); 
=======
    myId = getpid();
    sprintf(id, "%d", myId);
    const char *release = "3";
    char release_id[SIZE];
    strcpy(release_id, release);
    strcat(release_id, id);
>>>>>>> 73d981f66a014bdc749ea5ab0d24e2502bff65e2:TP3/client.c

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

    time_t end_time;
    time(&end_time);

    // Calculate the elapsed time
    double elapsed_time = difftime(end_time, start_time);
    printf("Elapsed time: %.2f seconds\n", elapsed_time);
}
