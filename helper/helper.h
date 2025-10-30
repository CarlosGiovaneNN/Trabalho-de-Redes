#ifndef HELPER_H
#define HELPER_H

#include "../router.h"

void add_to_inbound_queue(Package new_message);
void add_to_outbound_queue(Package new_message);

void remove_from_inbound_queue();
void remove_from_outbound_queue();

void print_queue(Queue *queue);

int read_configs();
void initialize_routing_tables();
void update_routing_table();
void print_tables();

void die(const char *s);

void send_neighbors_to_control_package();

#endif