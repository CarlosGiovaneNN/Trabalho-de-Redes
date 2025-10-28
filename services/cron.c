#include "../helper.h"
#include "common.h"

void *run_cron(void *arg)
{
    printf("Cron iniciado.\n");

    while (1)
    {
        usleep(time_to_control_package * 1000000); // tempo x segundos

        // printf("\nEnviando pacotes de controle...\n");

        send_neighbors_to_control_package();
    }

    return NULL;
}