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
};

// Fila circular para armazenar solicitações
struct Request requestQueue[100];
int front = -1;
int rear = -1;

// Mutex e variáveis de condição para sincronização
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
    //printf("%s \n", requestQueue[0].process_id);
    pthread_cond_signal(&queueNotEmptyCond);
    pthread_mutex_unlock(&queueMutex);
}

struct Request dequeueRequest() {
    pthread_mutex_lock(&queueMutex);

    if (isQueueEmpty()) {
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
    strcat(msg,id);
    sprintf(buffer, "%s", msg);    
    //printf("granted for %s \n", id);
    // Envie a mensagem de concessão para o cliente solicitante
    send(client_socket, buffer, strlen(buffer), 0);
    dequeueRequest();
}

void* moveQueue() {
    while (true){
    if (!isQueueEmpty()) {
        grant_lock(requestQueue[0]);
    }
    }
}

void* handle_client(void* arg) {
    //printf("created");
    struct Request request = *(struct Request*)arg;
    int client_socket = request.client_socket;
    char buffer[MAX_BUFFER_SIZE];
    char test[10] = "1111111111";

    while (true) {
        //printf("handleclient \n");
        int bytes_received = recv(client_socket, buffer, MAX_BUFFER_SIZE, 0);
        buffer[bytes_received] = '\0';
        //printf("%s recebido\n", buffer);
        // Verifique a mensagem de solicitação
        int num = strncmp(buffer, test, 1);
        if (strncmp(buffer, test, 1) == 0) { 
            enqueueRequest(request);
        }
        
    }
}

int main() {
    int coordinator_sock, client_sock;
    struct sockaddr_in coordinator_addr, client_addr;
    socklen_t client_addr_len;

    // Cria o socket TCP do coordenador
    if ((coordinator_sock = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("Falha na criação do socket...");
        exit(EXIT_FAILURE);
    }

    // Incorpora endereço do coordenador
    coordinator_addr.sin_family = AF_INET;
    coordinator_addr.sin_port = htons(COORDINATOR_PORT);
    coordinator_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    // Faz o bind para o endereço do coordenador
    if (bind(coordinator_sock, (struct sockaddr*)&coordinator_addr, sizeof(coordinator_addr)) == -1) {
        perror("Falha no binding...");
        exit(EXIT_FAILURE);
    }

    // Aguarda conexões de clientes
    if (listen(coordinator_sock, 5) == -1) {
        perror("Falha no listen...");
        exit(EXIT_FAILURE);
    }

    pthread_t threadID;

    // Inicia o algoritmo de exclusão mútua distribuída
    while (true) {
        // Aceita uma conexão de cliente
        client_sock = accept(coordinator_sock, (struct sockaddr*)&client_addr, &client_addr_len);
        if (client_sock == -1) {
            perror("Falha no ACCEPT!");
            continue;
        }

        // Cria uma nova thread para lidar com a solicitação do cliente
        struct Request* request = (struct Request*)malloc(sizeof(struct Request));
        request->client_socket = client_sock;

        // Recebe a mensagem de solicitação do cliente
        char buffer[MAX_BUFFER_SIZE];
        char test[10] = "1111111111";
        int bytes_received = recv(client_sock, buffer, MAX_BUFFER_SIZE, 0);
        buffer[bytes_received] = '\0';
        //printf("%s recebido\n", buffer);
        // Verifique a mensagem de solicitação
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
        enqueueRequest(*request);  // Adiciona a solicitação à fila

        if (pthread_create(&threadID, NULL, handle_client, (void*)request) != 0) {
            perror("Falha ao criar thread!");
            free(request);
            close(client_sock);
            continue;
        }

        pthread_detach(threadID);
        pthread_create(&threadID, NULL, moveQueue, NULL);

    }

    close(coordinator_sock);

    return 0;
}
