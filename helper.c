#include "services/common.h"

// funcao generica de adicionar mensagem a uma fila
void addToboundQueue(Queue *queue, Package newMessage)
{
    sem_wait(&queue->empty);

    pthread_mutex_lock(&(queue->mutex));
    queue->queue[queue->last] = newMessage;
    queue->last = (queue->last + 1) % QTY_ROUTERS;
    pthread_mutex_unlock(&(queue->mutex));

    sem_post(&queue->hasData);
}

void addToOutboundQueue(Package newMessage)
{
    addToboundQueue(&outbound, newMessage);
}

void addToInboundQueue(Package newMessage)
{
    addToboundQueue(&inbound, newMessage);
}

// funcao generica de remover mensagem de uma fila
void removeFromQueue(Queue *queue)
{
    memset(&queue->queue[queue->first], 0, sizeof(Package));
    queue->first = (queue->first + 1) % QTY_ROUTERS;
}

void removeFromInboundQueue()
{
    removeFromQueue(&inbound);
}

void removeFromOutboundQueue()
{
    removeFromQueue(&outbound);
}

// funcao generica de imprimir uma fila
void printQueue(Queue *queue)
{
    printf("\n\n----------------------\n");
    printf("\nFila de %s: \n", queue == &inbound ? "entrada" : "saida");
    printf("\n----------------------\n");

    for (int i = queue->first; i != queue->last; i = (i + 1) % QTY_ROUTERS)
    {
        Package message = queue->queue[i];
        printf("Tipo: %d\n", message.type);
        printf("Remetente: %d\n", message.sender);
        printf("Destinatario: %d\n", message.receiver);
        printf("Payload: %s\n", message.payload);
        printf("----------------------\n");
    }

    printf("----------------------\n\n");
}

// funcao de limpar os roteadores
void clearRouters()
{
    for (int i = 0; i < QTY_ROUTERS; i++)
    {
        if (routerId - 1 != i)
        {
            neighbors[i].id = -1;
            neighbors[i].cost = -1;
        }
        else
        {
            neighbors[i].id = routerId;
            neighbors[i].cost = 0;
        }
    }
}

// funcao de leitura dos arquivos de configuracao
int readConfigs()
{
    printf("Configurando o roteador....\n");
    FILE *arquivo;
    int numberLine = 1;
    char line[50];
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
                neighbors[router2 - 1].cost = cost;
            }
            else if (router2 == routerId)
            {
                neighbors[router1 - 1].cost = cost;
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

    numberLine = 1;

    while (fgets(line, sizeof(line), arquivo) != NULL)
    {
        int router, PORT;
        char ip[20];
        int qtyValues = sscanf(line, "%d %d %19s", &router, &PORT, ip);

        if (qtyValues == 3)
        {
            if (router == routerId)
            {
                port = PORT;
                strcpy(server, ip);
            }

            /// NOVA MANEIRA DE FAZER TESTANDO
            neighbors[router - 1].id = router;
            strcpy(neighbors[router - 1].ip, ip);
            neighbors[router - 1].port = PORT;
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
void die(const char *s)
{
    perror(s);
    exit(1);
}

void sendNeighborsToControlPackage()
{
    char messagePayload[PAYLOAD_SIZE] = "";

    for (int i = 0; i < QTY_ROUTERS; i++)
    {
        if (neighbors[i].id != -1)
        {
            char buffer[100];
            sprintf(buffer, "%d:%d;", neighbors[i].id, neighbors[i].cost);

            if (strlen(messagePayload) + strlen(buffer) < PAYLOAD_SIZE)
            {
                strcat(messagePayload, buffer);
            }
            else
            {
                printf("ERRO: payload estourou!\n");
                break;
            }
        }
    }

    for (int i = 0; i < QTY_ROUTERS; i++)
    {
        if (neighbors[i].id != routerId && neighbors[i].cost != -1 && neighbors[i].id != -1)
        {
            Package pkg;

            pkg.type = CONTROL;
            pkg.sender = routerId;
            pkg.receiver = neighbors[i].id;

            strcpy(pkg.payload, messagePayload);

            addToOutboundQueue(pkg);
        }
    }
}

//@todo
//Monta tudo zerado alterar para o vetor do roteador ja iniciar com 
//o que ele sabe dos seus vizinhos
void initializeRoutingTables(){
    for (int i = 0; i <  QTY_ROUTERS; i++) {
        routingTable[i].destination = i;
        
        if(i == routerId){
            routingTable[i].cost = 0;
            continue;
        }
        routingTable[i].cost = -1;
    }

    for (int i = 0; i < QTY_ROUTERS; i++) {
        for (int j = 0; j < QTY_ROUTERS; j++) {
            lastVectors[i][j].destination = j;
            lastVectors[i][j].cost = -1;
        }
    }
    
}
