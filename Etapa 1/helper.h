#ifndef HELPER_H
#define HELPER_H

void addToInboundQueue( char* buffer, int port, char* server);
void addToOutboundQueue(char* buffer, int port, char* server);

void removeFromInboundQueue();
void removeFromOutboundQueue();

void printQueue();

Router findRouterById(int id);

#endif