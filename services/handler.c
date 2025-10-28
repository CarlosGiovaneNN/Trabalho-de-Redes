#include "../helper.h"
#include "common.h"

void control_message_handler(Package package);
void data_message_handler(Package package);

void *run_handler(void *arg)
{
    printf("Handler iniciado.\n");

    while (1)
    {
        sem_wait(&inbound.has_data);

        pthread_mutex_lock(&inbound.mutex);

        pthread_mutex_lock(&console_mutex);

        Package package = inbound.queue[inbound.first];
        remove_from_inbound_queue();

        if (package.type == CONTROL)
        {
            control_message_handler(package);
        }
        else
        {
            data_message_handler(package);
        }

        pthread_mutex_unlock(&console_mutex);

        pthread_mutex_unlock(&inbound.mutex);

        sem_post(&inbound.empty);
    }

    return NULL;
}

///@todo -> montar uma função para recalcular o roteamento
/**
 * A função lê o pacote de controle e atualiza a
 * matriz de ultimos vetores salvos
 */
void control_message_handler(Package package)
{
    int origem = package.sender;
    char temp[PAYLOAD_SIZE];
    strcpy(temp, package.payload);

    char *token = strtok(temp, ";");
    while (token != NULL)
    {
        int id, cost;

        if (sscanf(token, "%d:%d", &id, &cost) == 2)
        {
            last_vectors[origem - 1][id - 1] = cost;
        }

        token = strtok(NULL, ";");
    }

    update_routing_table();        // atualiza a routing table
    data_message_handler(package); // mostra a mensagem de controle recebida
}

void data_message_handler(Package package)
{
    if (package.receiver == router_id)
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