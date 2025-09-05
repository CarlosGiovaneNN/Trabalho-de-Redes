#include "services/common.h"

// funcao generica de adicionar mensagem a uma fila
void addToboundQueue(Queue* queue, Package newMessage, int port, const char* server) {
    if (queue == NULL || queue->queue == NULL) {
        fprintf(stderr, "Queue inválida para mensagem %s:%d\n", server, port);
        return;
    }

    sem_wait(&queue->empty);

    pthread_mutex_lock(&(queue->mutex));
    queue->queue[queue->last] = newMessage;
    queue->last = (queue->last + 1) % QTY_ROUTERS;
    pthread_mutex_unlock(&(queue->mutex));

    sem_post(&queue->hasData);
}


void addToOutboundQueue(Package newMessage, int port, char* server) {
    addToboundQueue(&outbound, newMessage, port, server);
}

void addToInboundQueue( Package newMessage, int port,  char* server) {
    addToboundQueue(&inbound, newMessage, port, server);
}

// funcao generica de remover mensagem de uma fila
void removeFromQueue(Queue* queue) {
    memset(&queue->queue[queue->first], 0, sizeof(Package));
    queue->first = (queue->first + 1) % QTY_ROUTERS;
}

void removeFromInboundQueue() {
    removeFromQueue(&inbound);
}

void removeFromOutboundQueue() {
    removeFromQueue(&outbound);
}

// funcao generica de imprimir uma fila
void printQueue(Queue* queue) {
    printf("\n\n----------------------\n");
    printf("\nFila de %s: \n", queue == &inbound ? "entrada" : "saida");
    printf("\n----------------------\n");

    for (int i = queue->first; i != queue->last; i = (i + 1) % QTY_ROUTERS) {
        Package message = queue->queue[i];
        printf("Tipo: %d\n", message.type);
        printf("Remetente: %d\n", message.sender);
        printf("Destinatario: %d\n", message.receiver);
        printf("Payload: %s\n", message.payload);
        printf("----------------------\n");
    }
    
    printf("----------------------\n\n");
}

// funcao de encontrar um roteador pelo id, caso n encontre retorna um roteador com id -1
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

// funcao de inicializacao dos vizinhos
void initNeighbors()
{
    for (int i = 0; i < QTY_ROUTERS; i++)
    {
        neighbors[i] = -1;
    }
    neighbors[routerId - 1] = 0;
}

// funcao de limpar os roteadores
void clearRouters()
{
    for (int i = 0; i < QTY_ROUTERS; i++)
    {
        routers[i].id = -1;
    }
}

// funcao de leitura dos arquivos de configuracao
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
                neighbors[router2 - 1] = cost;
            }
            else if (router2 == routerId)
            {
                neighbors[router1 - 1] = cost;
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

// funcao de tratamento de erros
void die(const char *s) {
    perror(s);
    exit(1);
}