#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <semaphore.h>  //utilização utilizar o semaforo
#include <pthread.h>    //utilização threads e o mutex
#include <sys/socket.h> //utilização dos sockets de conexão a rede (Linux)
#include <arpa/inet.h>  //converter endereços de Internet (como endereços IP) entre seu formato de texto e seu formato numérico binário.
#include <unistd.h>

#include "services/common.h"

void addToboundQueue(Queue* queue, char* buffer, int port, char* server) {

      if (strlen(buffer) < 4) {
        printf("Mensagem inválida %s: %d\n", server,port);
        return;
    }

    Package newMessage;

    newMessage.type[0] = buffer[0];
    newMessage.sender = buffer[1] - '0';
    newMessage.receiver = buffer[2] - '0';

    // Copiar o payload a partir da posicao 3 ate completar o payload
    strncpy(newMessage.payload, &buffer[3], sizeof(newMessage.payload) - 1);
    newMessage.payload[sizeof(newMessage.payload) - 1] = '\0';

    // Deixa o espaco de buffer vazio
    memset(newMessage.buffer, 0, sizeof(newMessage.buffer));

    pthread_mutex_lock(&(queue->mutex));
    queue->queue[queue->last] = newMessage;
    queue->last = (queue->last + 1) % QTY_ROUTERS;
    pthread_mutex_unlock(&(queue->mutex));

}

void addToOutboundQueue(char* buffer, int port, char* server) {
    addToboundQueue(&outbound, buffer, port, server);
}

void addToInboundQueue( char* buffer, int port,  char* server) {
    addToboundQueue(&inbound, buffer, port, server);
}

void removeFromQueue(Queue* queue) {
    pthread_mutex_lock(&(queue->mutex));
    memset(&queue->queue[queue->first], 0, sizeof(Package));
    queue->first = (queue->first + 1) % QTY_ROUTERS;
    pthread_mutex_unlock(&(queue->mutex));
}

void removeFromInboundQueue() {
    removeFromQueue(&inbound);
}

void removeFromOutboundQueue() {
    removeFromQueue(&outbound);
}

void printQueue() {
    Queue* queue = &outbound;
    printf("\nFila de saida: \n");
    for (int i = queue->first; i != queue->last; i = (i + 1) % QTY_ROUTERS) {
        Package message = queue->queue[i];
        printf("Sender: %d, Receiver: %d, Payload: %s\n", message.sender, message.receiver, message.payload);
    }
}

Router findRouterById(int id) {
    for (int i = 0; i < qtyRouters; i++) {
        if (routers[i].id == id) {
            return routers[i];
        }
    }

    Router notFound;
    notFound.id = -1;

    return notFound;
}