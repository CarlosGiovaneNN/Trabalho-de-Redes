#include "../helper/helper.h"
#include "../router.h"

void control_message_handler(Package package);
void data_message_handler(Package package);

void *run_handler(void *arg)
{
    printf("Handler iniciado.\n");

    while (1)
    {
        sem_wait(&inbound.has_data);

        pthread_mutex_lock(&inbound.mutex);

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

        pthread_mutex_unlock(&inbound.mutex);

        sem_post(&inbound.empty);
    }

    return NULL;
}

/**
 * A função lê o pacote de controle e atualiza a
 * matriz de ultimos vetores salvos
 */
void control_message_handler(Package package)
{
    char temp[PAYLOAD_SIZE];
    strcpy(temp, package.payload);

    pthread_mutex_lock(&routers_mutex);

    time_t now;
    time(&now);

    neighbors[package.sender - 1].last_time_seen = now;

    if (neighbors[package.sender - 1].cost == -1)
    {
        neighbors[package.sender - 1].cost = neighbors[package.sender - 1].original_cost;
    }

    char *token = strtok(temp, ";");
    while (token != NULL)
    {
        int id, cost;

        if (sscanf(token, "%d:%d", &id, &cost) == 2)
        {
            last_vectors[package.sender - 1][id - 1] = cost;
        }

        token = strtok(NULL, ";");
    }

    pthread_mutex_unlock(&routers_mutex);

    update_routing_table();
    data_message_handler(package);
}

void data_message_handler(Package package)
{
    if (package.receiver == router_id)
    {
        pthread_mutex_lock(&console_mutex);

        if (package.type == DATA || show_control_package == 1)
        {
            printf("\n---------------\nPacote recebido!\n\n");
            printf("Tipo: %s\n", package.type == CONTROL ? "Control" : "Data");
            printf("Remetente: %d\n", package.sender);
            printf("Destinatario: %d\n", package.receiver);
            printf("Payload: %s\n", package.payload);
            printf("---------------\n\n");
        }

        pthread_mutex_unlock(&console_mutex);
    }
    else
    {

        pthread_mutex_lock(&console_mutex);

        if (package.type == DATA)
        {
            printf("\n--------------------------------\n");
            printf("Enviando pacote do %d para %d", package.sender, package.receiver);
            printf("\n--------------------------------\n");
        }

        pthread_mutex_unlock(&console_mutex);

        add_to_outbound_queue(package);
    }
}