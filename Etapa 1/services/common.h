#ifndef COMMON_H
#define COMMON_H

#include <pthread.h>
#include <semaphore.h>

#define QTY_ROUTERS 10

typedef struct {
    char type[1];       // 0 - controle / 1 - dados
    int sender; 
    int receiver;
    char payload[140];  // Dados
    char buffer[100];   // Espaço livre
} Package;

typedef struct {
    Package queue[QTY_ROUTERS];
    int first;
    int last;
    pthread_mutex_t mutex;
    sem_t sem_full;
    sem_t sem_empty;
} Queue;

extern Queue inbound;
extern Queue outbound;
extern int neighbors[QTY_ROUTERS];
extern int routerId;
extern char server[50];
extern int port;

#endif
