#include "common.h"
#include "../helper.h"
 
int showMenu();
void exitRouter();
void printStatus();
void sendPackage();
void showNeighbors();

void* run_shell(void* arg) {
    printf("Shell iniciado.\n\n----------------------\n");

    while(1) {
        int buffer = showMenu();

        if (buffer == 0) {
            exitRouter();
            break;

        } else if (buffer == 3) {
            showNeighbors();

        } else if (buffer == 2) {
            printStatus();

        } else if (buffer == 1)  {
            sendPackage();
            
        }
    }

    return NULL;
}

int showMenu() {
    char buffer[100];

    printf("\nDigite: \n1 - Enviar pacote.\n2 - Exibir status. \n3 - Mostrar vizinhos.\n0 - Sair.\n\n");
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
    printf("--------------------------\n\n");
}

void sendPackage() {
    char type[100], sendTo[100], payload[100];
    printf("Digite o tipo do pacote: \n0 - Controle\n1 - Dados\n\n->");
    scanf("%s", type);

    if(!strcmp(type, "0") == 0 && !strcmp(type, "1") == 0) {
        printf("Tipo inválido!\n\n");
        return;
    }

    printf("Digite o numero do roteador que deseja enviar o pacote:\n");
    for (int i = 0; i < QTY_ROUTERS; i++) {
        if (routers[i].id != routerId && routers[i].id != -1) {
            printf("%d - %s:%d\n", routers[i].id, routers[i].ip, routers[i].port);
        }
    }
    printf("\n->");
    scanf("%s", sendTo);

    int sendToId = atoi(sendTo);

    if((sendToId < 0 || sendToId == routerId)) {
        printf("Roteador inválido!\n\n");
        return;
    }

    Router sendToRouter = findRouterById(sendToId);

    if(sendToRouter.id == -1) {
        printf("Roteador não encontrado!\n\n");
        return;
    }

    printf("Digite o payload: ");
    printf("\n->");
    scanf("%s", payload);

    char package[BUFLEN];
    strcpy(package, type);


    char sender[ROUTER_ID_SIZE];
    sprintf(sender, "%d", routerId);
    strcat(package, sender);

    strcat(package, sendTo);
    strcat(package, payload);

    addToOutboundQueue(package, sendToRouter.port, sendToRouter.ip);
    printQueue(&outbound);
}

void showNeighbors() {
    for (int i = 0; i < QTY_ROUTERS; i++) {
        if (routers[i].id != routerId && routers[i].id != -1 && neighbors[i] != -1) {
            printf("Vizinho: %d - %s:%d, custo: %d\n", routers[i].id, routers[i].ip, routers[i].port, neighbors[i]);
        }
    }
    printf("\n----------------------\n");
}