#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>

#define COORDINATOR_PORT 8080
#define MAX_BUFFER_SIZE 1024

// Estrutura para informações de solicitação
struct Request {
    char process_id[9];
    int client_socket;
    bool current;
};

// Fila circular para armazenar solicitações
struct Request requestQueue[100];
int front = -1;
int rear = -1;

// Mutex e variável de condição para sincronização
pthread_mutex_t queueMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t queueNotEmptyCond = PTHREAD_COND_INITIALIZER;

bool isQueueEmpty() {
    return front == -1 && rear == -1;
}

void enqueueRequest(struct Request request) {
    pthread_mutex_lock(&queueMutex);

    if (isQueueEmpty()) {
        front = 0;
        rear = 0;
    } else {
        rear = (rear + 1) % 100;
    }
    requestQueue[rear] = request;
    pthread_cond_signal(&queueNotEmptyCond);
    pthread_mutex_unlock(&queueMutex);
}

struct Request dequeueRequest() {
    pthread_mutex_lock(&queueMutex);

    while (isQueueEmpty()) {
        pthread_cond_wait(&queueNotEmptyCond, &queueMutex);
    }

    struct Request request = requestQueue[front];

    if (front == rear) {
        front = -1;
        rear = -1;
    } else {
        front = (front + 1) % 100;
    }
    pthread_mutex_unlock(&queueMutex);

    return request;
}

void grant_lock(struct Request request) {
    char buffer[MAX_BUFFER_SIZE];
    int client_socket = request.client_socket;
    
    char id[9];
    strcpy(id, request.process_id);
    char msg[10];
    strcpy(msg, "2");
    strcat(msg, id);
    sprintf(buffer, "%s", msg);
    send(client_socket, buffer, strlen(buffer), 0);
    request.current = true;
    printf("Mensagem enviada grant: %s\n", buffer);
    char msg2[10];
    strcpy(msg2, "3");
    printf("Current:%s \n", request.process_id);
    strcat(msg2, id);
    char buffer2[MAX_BUFFER_SIZE];
    recv(client_socket, buffer2, MAX_BUFFER_SIZE, 0);
    request.current = false;
    printf("Mensagem recebida release: %s\n", buffer2);
    if (strncmp(buffer2,msg2,10))
    {
        dequeueRequest();
        
    }
}

void* moveQueue() {
    while (true) {
        pthread_mutex_lock(&queueMutex);

        while (isQueueEmpty()) {
            pthread_cond_wait(&queueNotEmptyCond, &queueMutex);
        }

        struct Request request = requestQueue[front];

        pthread_mutex_unlock(&queueMutex);

        grant_lock(request);

        dequeueRequest();
    }
}

void* handle_client(void* arg) {
    struct Request request = *(struct Request*)arg;
    int client_socket = request.client_socket;
    char buffer[MAX_BUFFER_SIZE];
    char test[10] = "1111111111";

    while (true) {
        if (request.current == false){
            int bytes_received = recv(client_socket, buffer, MAX_BUFFER_SIZE, 0);
            printf("Mensagem recebida pedido: %s\n", buffer);

            buffer[bytes_received] = '\0';

            int num = strncmp(buffer, test, 1);
            if (strncmp(buffer, test, 1) == 0) {
                enqueueRequest(request);
            }
        }
    }   
}

void* process_commands(void* arg) {
    char command[100];
    while (true) {
        printf("Digite um comando - 1 para ver a fila de request, 2 para ver o número de vezes que um processo foi atendido e 3 para encerrar o processo: ");
        fgets(command, sizeof(command), stdin);

        command[strcspn(command, "\n")] = '\0';

        if (strcmp(command, "1") == 0) {
            printf("Processos em fila de Request:\n");

            if (isQueueEmpty()) {
                printf("Empty\n");
            } else {
                int i = front;
                do {
                    printf("Process ID: %s\n", requestQueue[i].process_id);
                    i = (i + 1) % 100;
                } while (i != (rear + 1) % 100);
            }
        } else if (strcmp(command, "2") == 0) {
            // Implementar a lógica para rastrear o número de vezes que um processo de cliente teve uma solicitação atendida
        } else if (strcmp(command, "3") == 0) {
            exit(EXIT_SUCCESS);
        } else {
            printf("Comando inválido. Escolha 1, 2 ou 3.\n");
        }
    }
}

int main() {
    int coordinator_sock, client_sock;
    struct sockaddr_in coordinator_addr, client_addr;
    socklen_t client_addr_len;

    if ((coordinator_sock = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("Falha na criação do socket...");
        exit(EXIT_FAILURE);
    }

    coordinator_addr.sin_family = AF_INET;
    coordinator_addr.sin_port = htons(COORDINATOR_PORT);
    coordinator_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    if (bind(coordinator_sock, (struct sockaddr*)&coordinator_addr, sizeof(coordinator_addr)) == -1) {
        perror("Falha no binding...");
        exit(EXIT_FAILURE);
    }

    if (listen(coordinator_sock, 5) == -1) {
        perror("Falha no listen...");
        exit(EXIT_FAILURE);
    }

    pthread_t move_queue_thread;
    if (pthread_create(&move_queue_thread, NULL, moveQueue, NULL) != 0) {
        perror("Falha ao criar thread de movimento da fila!");
        exit(EXIT_FAILURE);
    }

    pthread_t command_thread;
    if (pthread_create(&command_thread, NULL, process_commands, NULL) != 0) {
        perror("Falha ao criar thread de processamento de comandos!");
        exit(EXIT_FAILURE);
    }

    while (true) {
        client_sock = accept(coordinator_sock, (struct sockaddr*)&client_addr, &client_addr_len);
        if (client_sock == -1) {
            perror("Falha no ACCEPT!");
            continue;
        }

        struct Request* request = (struct Request*)malloc(sizeof(struct Request));
        request->client_socket = client_sock;

        char buffer[MAX_BUFFER_SIZE];
        char test[10] = "1111111111";
        int bytes_received = recv(client_sock, buffer, MAX_BUFFER_SIZE, 0);
        printf("Mensagem recebida connect: %s\n", buffer);

        buffer[bytes_received] = '\0';

        int num = strncmp(buffer, test, 1);
        if (strncmp(buffer, test, 1) != 0) {
            perror("Mensagem de solicitação inválida!");
            free(request);
            close(client_sock);
            continue;
        }

        char process_id[9];
        strcpy(process_id, buffer + 1);
        strcpy(request->process_id, process_id);
        enqueueRequest(*request);

        pthread_t threadID;
        if (pthread_create(&threadID, NULL, handle_client, (void*)request) != 0) {
            perror("Falha ao criar thread!");
            free(request);
            close(client_sock);
            continue;
        }

        pthread_detach(threadID);
    }

    close(coordinator_sock);

    return 0;
}
