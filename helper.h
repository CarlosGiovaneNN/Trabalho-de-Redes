#ifndef HELPER_H
#define HELPER_H

void addToInboundQueue(Package newMessage, int port, char* server);
void addToOutboundQueue(Package newMessage, int port, char* server);

void removeFromInboundQueue();
void removeFromOutboundQueue();

void printQueue(Queue* queue);

void readConfigs();

void die(const char *s);

Router findRouterById(int id);

#endif