#include "common.h"
#include "../helper.h"

void* run_handler(void* arg) {
    printf("Handler iniciado.\n");

    
    while(1) {
        sem_wait(&inbound.hasData);

        pthread_mutex_lock(&inbound.mutex);

        pthread_mutex_lock(&console_mutex);
        printQueue(&inbound);
        pthread_mutex_unlock(&console_mutex);

        pthread_mutex_unlock(&inbound.mutex);

        removeFromInboundQueue();

        sem_post(&inbound.empty);
        
        usleep(1000);
    }
    return NULL;
}