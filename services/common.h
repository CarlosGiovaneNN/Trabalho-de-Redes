#ifndef COMMON_H
#define COMMON_H

#include <arpa/inet.h> //converter endereços de Internet (como endereços IP) entre seu formato de texto e seu formato numérico binário.
#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h> //utilização dos sockets de conexão a rede (Linux)
#include <unistd.h>

#include "cron.h"
#include "handler.h"
#include "receiver.h"
#include "sender.h"
#include "shell.h"

#define QTY_ROUTERS 10
#define BUFLEN sizeof(Package)
#define CONTROL 0
#define DATA 1
#define PAYLOAD_SIZE 140

#define PATH_CONFIG_ENLACES "configs/enlaces.config"
#define PATH_CONFIG_ROTERS "configs/roteador.config"

typedef struct
{
    int type;                   // 0 - controle / 1 - dados
    int sender;                 // Roteador de origem do pacote
    int receiver;               // Roteador destino do pacote
    char payload[PAYLOAD_SIZE]; // Dados
    char buffer[100];           // Espaço livre
} Package;

typedef struct
{
    Package queue[QTY_ROUTERS];
    int first;
    int last;
    pthread_mutex_t mutex;
    sem_t has_data;
    sem_t empty;
} Queue;

typedef struct
{
    char id;
    char ip[50];
    int port;
    int cost;
} Router;

typedef struct
{
    int destination;
    int cost;
    int next_router;
} RoutingTableEntry;

extern Queue inbound;
extern Queue outbound;
extern RoutingTableEntry routing_table[QTY_ROUTERS];
extern int last_vectors[QTY_ROUTERS][QTY_ROUTERS];
extern int router_id;
extern char server[50];
extern int port;
extern int time_to_control_package;
extern pthread_t thread_receiver, thread_sender, thread_handler, thread_shell;
extern Router neighbors[QTY_ROUTERS];
extern pthread_mutex_t console_mutex;

#endif
