#ifndef COMMON_H
#define COMMON_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <sys/socket.h> //utilização dos sockets de conexão a rede (Linux)
#include <arpa/inet.h>  //converter endereços de Internet (como endereços IP) entre seu formato de texto e seu formato numérico binário.
#include <unistd.h>

#include "shell.h"
#include "receiver.h"
#include "sender.h"
#include "handler.h"

#define QTY_ROUTERS 10
#define ROUTER_ID_SIZE 1
#define BUFLEN 512

#define pathConfigEnlaces "../configs/enlaces.config"
#define pathConfigRoteador "../configs/roteador.config"

typedef struct {
    char type[2];       // 0 - controle / 1 - dados
    char sender[ROUTER_ID_SIZE + 1]; 
    char receiver[ROUTER_ID_SIZE + 1];
    char payload[140];  // Dados
    char buffer[100];   // Espaço livre
} Package;

typedef struct {
    Package queue[QTY_ROUTERS];
    int first;
    int last; 
    pthread_mutex_t mutex;
    sem_t full;
    sem_t empty;
} Queue;

typedef struct {
    int id;
    char ip[50];
    int port;
} Router;

extern Queue inbound;
extern Queue outbound;
extern int neighbors[QTY_ROUTERS];
extern int routerId;
extern char server[50];
extern int port;
extern pthread_t thread_receiver, thread_sender, thread_handler,thread_shell;
extern Router routers[QTY_ROUTERS];

#endif
