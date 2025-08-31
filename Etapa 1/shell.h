#ifndef SHELL_H
#define SHELL_H

#include <pthread.h>

#define BUFLEN 512
#define qtyRouters 10

typedef struct
{
    char type[1];
    int sender;
    int receiver;
    char payload[140];
    char buffer[100];
} package;

//    Isso "avisa" ao shell.c que essas variáveis existem e são definidas em main.c
extern package inbound[10];
extern package outbound[10];
extern int neighbors[qtyRouters];
extern int routerId;
extern char server[50];
extern int port;

//    Esta é a função que você passará para pthread_create() na sua main. (declarada no shell.c)
void *rotina_shell(void *arg);

#endif