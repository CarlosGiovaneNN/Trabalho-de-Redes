#include "common.h"
#include "../helper.h"

void *run_cron(void *arg)
{
    printf("Cron iniciado.\n");

    while (1)
    {
        usleep(timeToControlPackage * 1000000); // tempo x segundos

        // printf("\nEnviando pacotes de controle...\n");

        sendNeighborsToControlPackage();
    }

    return NULL;
}