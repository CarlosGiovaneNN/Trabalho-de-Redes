#ifndef HELPER_H
#define HELPER_H

void addToInboundQueue(Package newMessage);
void addToOutboundQueue(Package newMessage);

void removeFromInboundQueue();
void removeFromOutboundQueue();

void printQueue(Queue *queue);

void readConfigs();
void initializeRoutingTables();

void die(const char *s);

void sendNeighborsToControlPackage();

#endif