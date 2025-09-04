#include "services/common.h"
#include "helper.h"

Queue inbound; //Fila de entrada
Queue outbound; //Fila de saida
int neighbors[QTY_ROUTERS];
int routerId = -1;
char server[50];
int port = 0;
pthread_t thread_receiver, thread_sender, thread_handler,thread_shell;
Router routers[QTY_ROUTERS];
pthread_mutex_t console_mutex;

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

    sem_init(&outbound.hasData, 0, 0);
    sem_init(&outbound.empty, 0, QTY_ROUTERS); 
    pthread_mutex_init(&outbound.mutex, NULL);

    sem_init(&inbound.hasData, 0, 0);
    sem_init(&inbound.empty, 0, QTY_ROUTERS);
    pthread_mutex_init(&inbound.mutex, NULL);
    
    if(pthread_create(&thread_receiver, NULL, &run_receiver, NULL) != 0) {
        printf("Falha ao criar a thread do receiver");
        return -1;
    }

    usleep(1000);
    if(pthread_create(&thread_sender, NULL, &run_sender, NULL) != 0) {
        printf("Falha ao criar a thread do sender");
        return -1;
    }

    usleep(1000);
    if(pthread_create(&thread_handler, NULL, &run_handler, NULL) != 0) {
        printf("Falha ao criar a thread do handler");
        return -1;
    }

    usleep(1000);
    if (pthread_create(&thread_shell, NULL, &run_shell, NULL) != 0)
    {
        printf("Falha ao criar a thread do shell");
        return -1;
    }

    pthread_join(thread_receiver, NULL);
    pthread_join(thread_sender, NULL);
    pthread_join(thread_handler, NULL);
    pthread_join(thread_shell, NULL);

    return 0;
}
