#include "common.h"
#include "../helper.h"

void controlMessageHandler(Package package);
void dataMessageHandler(Package package);

void *run_handler(void *arg)
{
    printf("Handler iniciado.\n");

    while (1)
    {
        sem_wait(&inbound.hasData);

        pthread_mutex_lock(&inbound.mutex);

        pthread_mutex_lock(&console_mutex);

        Package package = inbound.queue[inbound.first];
        removeFromInboundQueue();

        if (package.type == CONTROL)
        {
            controlMessageHandler(package);
        }
        else
        {
            dataMessageHandler(package);
        }

        pthread_mutex_unlock(&console_mutex);

        pthread_mutex_unlock(&inbound.mutex);

        sem_post(&inbound.empty);
    }

    return NULL;
}

void controlMessageHandler(Package package)
{
    char temp[PAYLOAD_SIZE];
    strcpy(temp, package.payload);

    char *token = strtok(temp, ";");
    while (token != NULL)
    {
        int id, cost;

        if (sscanf(token, "%d:%d", &id, &cost) == 2)
        {

            for (int i = 0; i < QTY_ROUTERS; i++)
            {
                if (neighbors[i].id == id && package.sender == id)
                {
                    neighbors[i].cost = cost;
                    break;
                }
            }
        }

        token = strtok(NULL, ";");
    }

    dataMessageHandler(package);
}

void dataMessageHandler(Package package)
{
    if (package.receiver == routerId)
    {
        printf("\n---------------\nPacote recebido!\n---------------\n");
        printf("Tipo: %d\n", package.type);
        printf("Remetente: %d\n", package.sender);
        printf("Destinatario: %d\n", package.receiver);
        printf("Payload: %s\n", package.payload);
        printf("---------------\n\n");
    }
    else
    {
    }
}