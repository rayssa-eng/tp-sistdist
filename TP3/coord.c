#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>

#define COORDINATOR_PORT 5000
#define MAX_BUFFER_SIZE 1024

// Structure for request information
struct Request {
    int process_id;
    int client_socket;
};

// Circular queue for storing requests
struct Request requestQueue[100];
int front = -1;
int rear = -1;

// Mutex and condition variables for synchronization
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

void grant_lock(int client_socket) {
    char buffer[MAX_BUFFER_SIZE];
    sprintf(buffer, "GRANT");

    // Send grant message to the requesting client
    send(client_socket, buffer, strlen(buffer), 0);
}

void* handle_client(void* arg) {
    struct Request request = *(struct Request*)arg;
    int client_socket = request.client_socket;

    // Process the request and send grant
    grant_lock (client_socket);

    // Clean up the client connection
    close(client_socket);
    free(arg);

    pthread_exit(NULL);
}

int main() {
    int coordinator_sock, client_sock;
    struct sockaddr_in coordinator_addr, client_addr;
    socklen_t client_addr_len;

    // cria o socket TCP do coordenador
    if ((coordinator_sock = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("Falha na criação do socket...");
        exit(EXIT_FAILURE);
    }

    // incorpora endereco do coordenador
    coordinator_addr.sin_family = AF_INET;
    coordinator_addr.sin_port = htons(COORDINATOR_PORT);
    coordinator_addr.sin_addr.s_addr = INADDR_ANY;

    // bind pro endereço do coordenador
    if (bind(coordinator_sock, (struct sockaddr*)&coordinator_addr, sizeof(coordinator_addr)) == -1) {
        perror("Falha no binding...");
        exit(EXIT_FAILURE);
    }

    // Listen for incoming client connections
    if (listen(coordinator_sock, 5) == -1) {
        perror("Falha no listen...");
        exit(EXIT_FAILURE);
    }

    pthread_t threadID;

    // Start the distributed mutual exclusion algorithm
    while (true) {
        // Accept a client connection
        client_sock = accept(coordinator_sock, (struct sockaddr*)&client_addr, &client_addr_len);
        if (client_sock == -1) {
            perror("Falha no ACCEPT!");
            continue;
        }

        // Create a new thread to handle the client request
        struct Request* request = (struct Request*)malloc(sizeof(struct Request));
        request->client_socket = client_sock;

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