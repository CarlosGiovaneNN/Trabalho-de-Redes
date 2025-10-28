#include "../helper.h"
#include "common.h"

void send_package_to_router(Package package);

void *run_sender(void *arg)
{
    printf("Sender iniciado.\n");

    while (1)
    {
        // aq fica esperando a fila de saida ter um item
        sem_wait(&outbound.has_data);

        pthread_mutex_lock(&outbound.mutex);

        // remove o pacote da fila
        Package package_to_send = outbound.queue[outbound.first];
        remove_from_outbound_queue();

        pthread_mutex_unlock(&outbound.mutex);

        sem_post(&outbound.empty);

        // envia o pacote
        send_package_to_router(package_to_send);
        printf(package_to_send.type == CONTROL ? "\n" : "\n---------------\nPacote enviado!\n---------------\n");

        usleep(1000);
    }
    return NULL;
}

void send_package_to_router(Package package)
{
    if (package.type == DATA)
    {
        printf("\nEnviando pacote...\n");
    }

    // configuracoes do socket
    struct sockaddr_in si_other;
    int s, i, slen = sizeof(si_other);
    char buf[BUFLEN];
    char message[BUFLEN];

    Router router = neighbors[package.receiver - 1];
    if (router.id == -1)
    {
        printf("Roteador não encontrado!\n\n");
        return;
    }
    else if (router.cost == -1 && package.type == DATA)
    {
        printf("Este roteador não é um vizinho!\n\n");
        return;
    }

    if ((s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
    {
        die("socket");
    }

    memset((char *)&si_other, 0, sizeof(si_other));
    si_other.sin_family = AF_INET;
    si_other.sin_port = htons(router.port);

    if (inet_aton(router.ip, &si_other.sin_addr) == 0)
    {
        fprintf(stderr, "inet_aton() failed\n");
        exit(1);
    }

    // envia o pacote
    if (sendto(s, &package, sizeof(package), 0, (struct sockaddr *)&si_other, slen) == -1)
    {
        die("sendto()");
    }

    close(s);
}
