#include "services/common.h"
#include "helper.h"

Queue inbound;  // Fila de entrada
Queue outbound; // Fila de saida
int routerId = -1;
char server[50];
int port = 0;
int timeToControlPackage = 10;
pthread_t thread_receiver, thread_sender, thread_handler, thread_shell, thread_cron;
Router neighbors[QTY_ROUTERS]; // Armazena os dados de todos os roteadores e os vizinhos são representados por aqueles que tem um cost > 0
RoutingTableEntry routingTable[QTY_ROUTERS];
RoutingTableEntry lastVectors[QTY_ROUTERS][QTY_ROUTERS];
pthread_mutex_t console_mutex;
pthread_mutex_t routers_mutex;

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
        if (routerId <= 0 || routerId > QTY_ROUTERS)
        {
            printf("ID do roteador invalido\n");
            return 1;
        }
    }

    // Configurando o roteador e seus vizinhos
    readConfigs();
    // Inicializando a tabela de roteamento e a matriz dos vetores recebidos
    initializeRoutingTables();

    // GPT me falou que tem que ser assim um mutex recursivo
    pthread_mutexattr_t attr;
    pthread_mutexattr_init(&attr);
    pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE_NP);

    // Inicia o mutex das filas de entrada e saída
    pthread_mutex_init(&outbound.mutex, &attr);
    pthread_mutex_init(&inbound.mutex, &attr);

    pthread_mutex_init(&console_mutex, &attr);
    pthread_mutex_init(&routers_mutex, &attr);

    pthread_mutexattr_destroy(&attr);

    // inicia semaforos
    sem_init(&outbound.hasData, 0, 0);
    sem_init(&outbound.empty, 0, QTY_ROUTERS);

    sem_init(&inbound.hasData, 0, 0);
    sem_init(&inbound.empty, 0, QTY_ROUTERS);

    // cria as e inicia as threads
    if (pthread_create(&thread_receiver, NULL, &run_receiver, NULL) != 0)
    {
        printf("Falha ao criar a thread do receiver");
        return -1;
    }

    usleep(1000);
    if (pthread_create(&thread_sender, NULL, &run_sender, NULL) != 0)
    {
        printf("Falha ao criar a thread do sender");
        return -1;
    }

    usleep(1000);
    if (pthread_create(&thread_handler, NULL, &run_handler, NULL) != 0)
    {
        printf("Falha ao criar a thread do handler");
        return -1;
    }

    usleep(1000);
    if (pthread_create(&thread_cron, NULL, &run_cron, NULL) != 0)
    {
        printf("Falha ao criar a thread do cron");
        return -1;
    }

    usleep(1000);
    if (pthread_create(&thread_shell, NULL, &run_shell, NULL) != 0)
    {
        printf("Falha ao criar a thread do shell");
        return -1;
    }

    // aguarda as threads, assim que forem finalizadas a main continua
    if (pthread_join(thread_receiver, NULL) != 0)
    {
        printf("Falha ao juntar a thread do receiver");
        return -1;
    }

    if (pthread_join(thread_sender, NULL) != 0)
    {
        printf("Falha ao juntar a thread do sender");
        return -1;
    }

    if (pthread_join(thread_handler, NULL) != 0)
    {
        printf("Falha ao juntar a thread do handler");
        return -1;
    }

    if (pthread_join(thread_cron, NULL) != 0)
    {
        printf("Falha ao juntar a thread do cron");
        return -1;
    }

    if (pthread_join(thread_shell, NULL) != 0)
    {
        printf("Falha ao juntar a thread do shell");
        return -1;
    }

    return 0;
}
