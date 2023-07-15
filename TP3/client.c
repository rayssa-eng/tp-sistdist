#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
   
#define PORT 8080
#define MAXLINE 1024
#define SIZE 10
#define r 10
#define k 4

int sockfd;
int myId;
char id[2];
char msg[SIZE];

// Driver code
int main() {
    myId = getpid();
    itoa(myId, id, 10);
    
     const char *request = strcat("1", id);
    const char *grant = strcat("2", id);
    const char *release = strcat("3", id);

    struct sockaddr_in     servaddr;
    if ( (sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }
    memset(&servaddr, 0, sizeof(servaddr));  
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(PORT);
    servaddr.sin_addr.s_addr = INADDR_ANY;
    my_run();
}

int get_acess()
{
    sendto(sockfd, (const char *)request, strlen(request),
        MSG_CONFIRM, (const struct sockaddr *) &servaddr, 
            sizeof(servaddr));
    int msg;
    msg = recvfrom(sockfd, (char *)buffer, MAXLINE, 
                MSG_WAITALL, (struct sockaddr *) &servaddr,
                &len);
    if (buffer==grant)
    {
        return 1;
    }
    else
    {
        return -1;
    }
}

void my_run()
{ int i = 0;
    while (i<r){
    if (get_acess == 1)
    {
        //abre arquivo
        myFile = fopen("resultado.txt", "a");
        //escreve hora + pid
        time_t current_time;
        char* c_time_string;
        current_time = time(NULL);
        c_time_string = ctime(&current_time);
        fprintf(myFile, "%d %s", id, c_time_string);
        //fecha arquivo
        fclose(myFile);

        sleep(k);
        sendto(sockfd, (const char *)release, strlen(release),
        MSG_CONFIRM, (const struct sockaddr *) &servaddr, sizeof(servaddr));
        i++;
    }
    }
}