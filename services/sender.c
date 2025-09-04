#include "common.h"
#include "../helper.h"

void sendPackageToRouter(Package package);

void* run_sender(void* arg) {
    printf("Sender iniciado.\n");

    while(1) {
        sem_wait(&outbound.hasData);

        pthread_mutex_lock(&outbound.mutex);

        Package packageToSend = outbound.queue[outbound.first];
        removeFromOutboundQueue();

        pthread_mutex_unlock(&outbound.mutex);

        sem_post(&outbound.empty);

        sendPackageToRouter(packageToSend);
        printf("Pacote enviado!\n");

        usleep(1000);
    }
    return NULL;
}

 
void sendPackageToRouter(Package package)
{
    printf("Enviando pacote...\n");
    struct sockaddr_in si_other;
    int s, i, slen = sizeof(si_other);
    char buf[BUFLEN];
    char message[BUFLEN];

    Router router = findRouterById(package.receiver);

    if(router.id == -1) {
        printf("Roteador não encontrado!\n\n");
        return;
    }
 
    if ( (s=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
    {
        die("socket");
    }
 
    memset((char *) &si_other, 0, sizeof(si_other));
    si_other.sin_family = AF_INET;
    si_other.sin_port = htons(router.port);


    if (inet_aton(router.ip , &si_other.sin_addr) == 0) 
    {
        fprintf(stderr, "inet_aton() failed\n");
        exit(1);
    }
     
    //send the message
    if (sendto(s, &package, sizeof(package) , 0 , (struct sockaddr *) &si_other, slen)==-1)
    {
        die("sendto()");
    }
 
    close(s);
}
