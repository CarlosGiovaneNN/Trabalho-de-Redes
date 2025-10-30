#include "../helper.h"
#include "common.h"

void calculate_timeout();

void *run_cron(void *arg)
{
    printf("Cron iniciado.\n");

    while (1)
    {
        usleep(time_to_control_package * 1000000); // tempo x segundos

        calculate_timeout();

        send_neighbors_to_control_package();
    }

    return NULL;
}

void calculate_timeout()
{
    pthread_mutex_lock(&routers_mutex);

    int table_changed = 0;

    time_t now;
    time(&now);

    for (int i = 0; i < QTY_ROUTERS; i++)
    {
        if (neighbors[i].cost != -1 && neighbors[i].id != router_id && neighbors[i].last_time_seen != 0)
        {
            double seconds_elapsed = difftime(now, neighbors[i].last_time_seen);

            if (seconds_elapsed > (time_to_control_package * 3))
            {
                printf("Timeout - o vizinho id: %d foi considerado morto\n", neighbors[i].id);

                neighbors[i].cost = -1;
                neighbors[i].last_time_seen = 0;

                table_changed = 1;
            }
        }
    }

    pthread_mutex_unlock(&routers_mutex);

    if (table_changed)
    {
        update_routing_table();
    }
}