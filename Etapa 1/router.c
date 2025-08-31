#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <semaphore.h>  //utilização utilizar o semaforo
#include <pthread.h>    //utilização threads e o mutex
#include <sys/socket.h> //utilização dos sockets de conexão a rede (Linux)
#include <arpa/inet.h>  //converter endereços de Internet (como endereços IP) entre seu formato de texto e seu formato numérico binário.

#define pathConfigEnlaces "../configs/enlaces.config"
#define pathConfigRoteador "../configs/roteador.config"
#define BUFLEN 512
#define qtyRouters 10

int readConfigs();
void initNeighbors();

typedef struct
{
    char type[1];
    int sender;
    int receiver;
    char payload[140];
    char buffer[100];
} Package;

Package inbound[10];
Package outbound[10];
int neighbors[qtyRouters];
int routerId = -1;
char server[50];
int port = 0;

int main(int argc, char const *argv[])
{
    if (argc != 2)
    {
        printf("Erro: Numero incorreto de argumentos!\n");
        printf("Para executar o router digite ./router idDoRoteador\n");
        printf(" ");
        return 1;
    }
    else
    {
        routerId = atoi(argv[1]);
        if (routerId < 0)
        {
            printf("ID do roteador invalido\n");
            return 1;
        }
    }

    readConfigs();
    return 0;
}

int readConfigs()
{
    printf("Configurando o roteador....\n");
    FILE *arquivo;
    int numberLine;
    char line[50];
    initNeighbors();

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
            fprintf(stderr, "Linha %d mal formatada ou vazia. Ignorando-a.\n", numberLine);
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
        }
        else
        {
            fprintf(stderr, "Linha %d mal formatada ou vazia. Ignorando-a.\n", numberLine);
        }

        numberLine++;
    }

    fclose(arquivo);

    printf("Configurado com sucesso\n");
    return 0;
}

void initNeighbors()
{
    for (int i = 0; i < qtyRouters; i++)
    {
        neighbors[i] = -1;
    }
    neighbors[routerId - 1] = 0;
}
