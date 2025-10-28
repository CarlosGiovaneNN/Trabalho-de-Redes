#include "services/common.h"
#include "helper.h"
#include <stdio.h>

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
        char buffer[100];
        sprintf(buffer, "%d:%d;", routingTable[i].destination, routingTable[i].cost);

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

/*
Inicializa a tabela de roteamento para cada destino possível
com custo -1 e proximo roteador -1, depois atualiza com o valor conhecido de seus vizinhos
Já o vetor recebido inicializa tudo com -1, menos o do proprio roteador
*/
void initializeRoutingTables() {

    for (int i = 0; i < QTY_ROUTERS; i++) {
        routingTable[i].destination = i+1;
        routingTable[i].cost = -1;
        routingTable[i].nextRouter = -1;
    }

    routingTable[routerId - 1].cost = 0;
    routingTable[routerId - 1].nextRouter = routerId;

    for (int i = 0; i < QTY_ROUTERS; i++) {
        if (neighbors[i].cost > 0) {

            routingTable[i].cost = neighbors[i].cost;
            routingTable[i].nextRouter = i+1;
        }
    }
    
   for (int i = 0; i < QTY_ROUTERS; i++) {
        for (int j = 0; j < QTY_ROUTERS; j++) {
            lastVectors[i][j] = -1;
        }
    }

    for (int i = 0; i < QTY_ROUTERS; i++) {
        lastVectors[routerId - 1][i] = routingTable[i].cost;
    }
}

/**
 * @todo
 */
void updateRoutingTable(){
    int changeTable = 0;

    for (int i = 0; i < QTY_ROUTERS; i++)
    {
        if(i == routerId - 1){
            continue;
        }

        for (int j = 0; j < QTY_ROUTERS; j++)
        {
            int lowerCost = routingTable[j].cost;
            int currentCost = -1;
            if(lastVectors[i][j] > 0 && neighbors[i].cost > 0){
                currentCost = lastVectors[i][j] + neighbors[i].cost;
            }

            if((currentCost < lowerCost || lowerCost == -1) && currentCost != 1){
                changeTable = 1;
                routingTable[j].cost = currentCost;
            }
        }
    }

    if(changeTable){
        //Aqui mudou a tabela tem q mandar o vetor de novo pros vizinhos
    }
    
}

/**
 * Peguei do gpt pq queria ver como tinha ficado as tabelas, depois da pra apagar 
 * @brief Imprime a tabela de roteamento atual e a matriz de vetores de distância
 * recebidos de forma formatada e segura para threads.
 */
void print_tables()
{
    // Trava o mutex do console para garantir que a impressão não seja interrompida
    pthread_mutex_lock(&console_mutex);

    printf("\n\n############################################################\n");
    printf("###               VISUALIZACAO DAS TABELAS (Roteador %d)    ###\n", routerId);
    printf("############################################################\n\n");

    // --- Imprimindo a Tabela de Roteamento Principal ---
    printf("=== Tabela de Roteamento ATUAL ===\n");
    printf("+-------------+---------+---------------+\n");
    printf("|  Destino    |  Custo  | Proximo Salto |\n");
    printf("+-------------+---------+---------------+\n");

    for (int i = 0; i < QTY_ROUTERS; i++)
    {
        printf("|      %2d     |", routingTable[i].destination);

        // Imprime o custo de forma amigável (mostra 'inf' para infinito)
        if (routingTable[i].cost == -1) {
            printf("   inf   |");
        } else {
            printf("   %3d   |", routingTable[i].cost);
        }

        // Imprime o próximo salto (mostra '-' para indefinido)
        if (routingTable[i].nextRouter == -1) {
            printf("       -       |");
        } else {
            printf("      %2d       |", routingTable[i].nextRouter);
        }

        // Adiciona um comentário para a rota até si mesmo
        if (routingTable[i].destination == routerId) {
            printf(" <- (Este roteador)");
        }
        printf("\n");
    }
    printf("+-------------+---------+---------------+\n\n\n");


    // --- Imprimindo a Matriz de Vetores Recebidos ---
    printf("=== Matriz de Vetores de Distancia Recebidos (lastVectors) ===\n");
    printf("         (Visao de Custo para cada Destino Dst)\n");

    // Cabeçalho da matriz (Destinos)
    printf("De \\ Dst |");
    for (int j = 0; j < QTY_ROUTERS; j++) {
        printf("  %2d  |", j + 1);
    }
    printf("\n---------+");
    for (int j = 0; j < QTY_ROUTERS; j++) {
        printf("------+");
    }
    printf("\n");

    // Corpo da matriz (um roteador por linha)
    for (int i = 0; i < QTY_ROUTERS; i++)
    {
        // Só imprime a linha se for um vizinho ou o próprio roteador
        if (neighbors[i].cost != -1) {
            printf(" Rota %2d |", neighbors[i].id); // Cabeçalho da linha (Quem enviou o vetor)
            for (int j = 0; j < QTY_ROUTERS; j++)
            {
                // Imprime o custo que o roteador 'i+1' anunciou para o destino 'j+1'
                int cost = lastVectors[i][j];
                if (cost == -1) {
                    printf("  inf |");
                } else {
                    printf("  %3d |", cost);
                }
            }
            if (neighbors[i].id == routerId) {
                printf(" <- (Meu proprio vetor)");
            }
            printf("\n");
        }
    }
    printf("------------------------------------------------------------------------\n\n");


    // Libera o mutex do console
    pthread_mutex_unlock(&console_mutex);
}
