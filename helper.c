#include "services/common.h"

void addToboundQueue(Queue* queue, char* buffer, int port, char* server) {

      if (strlen(buffer) < (1 + 2 * ROUTER_ID_SIZE)) {
        printf("Mensagem inválida %s: %d\n", server,port);
        return;
    }

    Package newMessage;

    printf( "Mensagem recebida %s: %d\n", server, port);
    printf("%s\n", buffer);

    strncpy(newMessage.type, &buffer[0], 1);
    newMessage.type[1] = '\0';
    printf("Tipo: %s\n", newMessage.type);

    strncpy(newMessage.sender, &buffer[1], ROUTER_ID_SIZE );
    newMessage.sender[ROUTER_ID_SIZE] = '\0';
    printf("Remetente: %s\n", newMessage.sender);

    strncpy(newMessage.receiver, &buffer[ROUTER_ID_SIZE + 1], ROUTER_ID_SIZE) ;
    newMessage.receiver[ROUTER_ID_SIZE] = '\0';
    printf("Destinatario: %s\n", newMessage.receiver);

    strncpy(newMessage.payload, &buffer[2 * ROUTER_ID_SIZE + 1], sizeof(newMessage.payload) - 1);
    newMessage.payload[sizeof(newMessage.payload) - 1] = '\0';
    printf("Payload: %s\n", newMessage.payload);

    // Deixa o espaco de buffer vazio
    memset(newMessage.buffer, 0, sizeof(newMessage.buffer));

    sem_wait(&queue->empty);

    pthread_mutex_lock(&(queue->mutex));
    queue->queue[queue->last] = newMessage;
    queue->last = (queue->last + 1) % QTY_ROUTERS;
    pthread_mutex_unlock(&(queue->mutex));

    sem_post(&queue->hasData);
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

void printQueue(Queue* queue) {
    printf("\n\n----------------------\n");
    printf("\nFila de %s: \n", queue == &inbound ? "entrada" : "saida");
    for (int i = queue->first; i != queue->last; i = (i + 1) % QTY_ROUTERS) {
        Package message = queue->queue[i];
        printf("Tipo: %s\n", message.type);
        printf("Remetente: %s\n", message.sender);
        printf("Destinatario: %s\n", message.receiver);
        printf("Payload: %s\n", message.payload);
    }
    printf("----------------------\n\n");
}

Router findRouterById(int id) {
    for (int i = 0; i < QTY_ROUTERS; i++) {
        if (routers[i].id == id) {
            return routers[i];
        }
    }

    Router notFound;
    notFound.id = -1;

    return notFound;
}


void initNeighbors()
{
    for (int i = 0; i < QTY_ROUTERS; i++)
    {
        neighbors[i] = -1;
    }
    neighbors[routerId - 1] = 0;
}

void clearRouters()
{
    for (int i = 0; i < QTY_ROUTERS; i++)
    {
        routers[i].id = -1;
    }
}

int readConfigs()
{
    printf("Configurando o roteador....\n");
    FILE *arquivo;
    int numberLine = 1;
    char line[50];
    initNeighbors();
    clearRouters();

    // iniciando a leitura do arquivo de enlaces.config
    arquivo = fopen(pathConfigEnlaces, "r");
    if (arquivo == NULL)
    {
        printf("Erro ao abrir o arquivo %s\n", pathConfigEnlaces);
        return 1;
    }

    while (fgets(line, sizeof(line), arquivo) != NULL)
    {
        int router1, router2, cost;
        int qtyValues = sscanf(line, "%d %d %d", &router1, &router2, &cost);

        if (qtyValues == 3)
        {
            if (router1 == routerId)
            {
                neighbors[router2] = cost;
            }
            else if (router2 == routerId)
            {
                neighbors[router1] = cost;
            }
        }
        else
        {
            printf("Caminhos: Linha %d mal formatada ou vazia. Ignorando-a.\n", numberLine);
        }

        numberLine++;
    }

    fclose(arquivo);

    // iniciando a leitura do arquivo roteador.config
    arquivo = fopen(pathConfigRoteador, "r");
    if (arquivo == NULL)
    {
        printf("Erro ao abrir o arquivo %s\n", pathConfigRoteador);
        return 1;
    }

    int routerindex = 0;
    numberLine = 1;

    while (fgets(line, sizeof(line), arquivo) != NULL)
    {
        int router, PORT;
        char ip[20];
        int qtyValues = sscanf(line, "%d %d %19s", &router, &PORT, ip);

        if (qtyValues == 3)
        {
            routers[routerindex].id = router;
            strcpy(routers[routerindex].ip, ip);
            routers[routerindex].port = PORT;
            routerindex++;
            if (router == routerId)
            {
                port = PORT;
                strcpy(server, ip);
            } 
        }
        else
        {
            printf("Roteadores: Linha %d mal formatada ou vazia. Ignorando-a.\n", numberLine);
        }

        numberLine++;
    }

    fclose(arquivo);

    printf("Configurado com sucesso\n");
    printf("--------------------------\n");
    return 0;
}

void die(const char *s) {
    perror(s);
    exit(1);
}