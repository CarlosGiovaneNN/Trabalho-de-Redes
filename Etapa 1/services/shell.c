#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "common.h"
#include "../helper.h"

int showMenu() {
    char buffer[100];

    printf("Digite: \n1 - Enviar pacote.\n2 - Exibir status. \n0 - Sair.\n");
    printf("router %d -> ", routerId);
    scanf("%s", buffer);

    return atoi(buffer);
}

void exitRouter() {
    pthread_cancel(thread_receiver);
    pthread_cancel(thread_sender);
    pthread_cancel(thread_handler);

    printf("Encerrando...\n");
}

void printStatus() {
    printf("--- Status do Roteador ---\n");
    printf("ID: %d\n", routerId);
    printf("IP: %s\n", server);
    printf("Porta: %d\n", port);
    printf("--------------------------\n");
}

void sendPackage() {
    char type[100], sendTo[100], payload[100];
    printf("Digite o tipo do pacote: \n0 - Controle\n1 - Dados\n");
    scanf("%s", type);

    if(!strcmp(type, "0") == 0 && !strcmp(type, "1") == 0) {
        printf("Tipo inválido!\n");
        return;
    }

    printf("Para qual roteador deseja enviar o pacote: ");
    scanf("%s", sendTo);

    int sendToId = atoi(sendTo);

    if((sendToId < 0 && sendToId == routerId)) {
        printf("Roteador inválido!\n");
        return;
    }

    Router sendToRouter = findRouterById(sendToId);

    if(sendToRouter.id == -1) {
        printf("Roteador não encontrado!\n");
        return;
    }

    printf("Digite o payload: ");
    scanf("%s", payload);

    char package[BUFLEN];
    strcpy(package, type);
    strcat(package, routerId);
    strcat(package, sendTo);
    strcat(package, payload);

    addToOutboundQueue(package, sendToRouter.port, sendToRouter.ip);
    printQueue();
}

void* run_shell(void* arg) {
    printf("Shell iniciado.\n");

    while(1) {
        int buffer = showMenu();

        if (buffer == 0) {
            exitRouter();
            break;

        } else if (buffer == 2) {
            printStatus();

        } else if (buffer == 1)  {
            sendPackage();

        }
    }

    return NULL;
}