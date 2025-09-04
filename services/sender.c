#include "common.h"
#include "../helper.h"

void sendPackageToRouter(Package package);

void* run_sender(void* arg) {
    printf("Sender iniciado.\n");

    while(1) {
        sem_wait(&outbound.hasData);

        pthread_mutex_lock(&outbound.mutex);

        Package packageToSend = outbound.queue[outbound.first];

        pthread_mutex_unlock(&outbound.mutex);
        
        removeFromOutboundQueue();

        sem_post(&outbound.empty);

        sendPackageToRouter(packageToSend);
        
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

    //create the message
    strcpy(message, package.type);

    char senderStr[ROUTER_ID_SIZE + 1];
    sprintf(senderStr, "%0*d", ROUTER_ID_SIZE, package.sender);
    printf("Sender: %s\n", senderStr);

    strcat(message, senderStr);

    char receiverStr[ROUTER_ID_SIZE + 1];
    sprintf(receiverStr, "%0*d", ROUTER_ID_SIZE, package.receiver);
    printf("Receiver: %s\n", receiverStr);

    strcat(message, package.receiver);
    
    strcat(message, package.payload);

    Router router = findRouterById(atoi(package.receiver));
    //printf("%d %s %d\n", router.id, router.ip, router.port);
    printf("%s", message);

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
    if (sendto(s, message, strlen(message) , 0 , (struct sockaddr *) &si_other, slen)==-1)
    {
        die("sendto()");
    }
        
    //receive a reply and print it
    //clear the buffer by filling null, it might have previously received data
    memset(buf,'\0', BUFLEN);
    //try to receive some data, this is a blocking call
    if (recvfrom(s, buf, BUFLEN, 0, (struct sockaddr *) &si_other, &slen) == -1)
    {
        die("recvfrom()");
    }
        
    puts(buf);
    
 
    close(s);
}
