#include "services/common.h"

// funcao generica de adicionar mensagem a uma fila
void add_to_queue(Queue *queue, Package new_message)
{
    sem_wait(&queue->empty);

    pthread_mutex_lock(&(queue->mutex));
    queue->queue[queue->last] = new_message;
    queue->last = (queue->last + 1) % QTY_ROUTERS;
    pthread_mutex_unlock(&(queue->mutex));

    sem_post(&queue->has_data);
}

void add_to_outbound_queue(Package new_message)
{
    add_to_queue(&outbound, new_message);
}

void add_to_inbound_queue(Package new_message)
{
    add_to_queue(&inbound, new_message);
}

// funcao generica de remover mensagem de uma fila
void remove_from_queue(Queue *queue)
{
    memset(&queue->queue[queue->first], 0, sizeof(Package));
    queue->first = (queue->first + 1) % QTY_ROUTERS;
}

void remove_from_inbound_queue()
{
    remove_from_queue(&inbound);
}

void remove_from_outbound_queue()
{
    remove_from_queue(&outbound);
}

// funcao generica de imprimir uma fila
void print_queue(Queue *queue)
{
    printf("\n\n----------------------\n");
    printf("\nFila de %s: \n", queue == &inbound ? "entrada" : "saida");
    printf("\n----------------------\n");

    for (int i = queue->first; i != queue->last; i = (i + 1) % QTY_ROUTERS)
    {
        Package message = queue->queue[i];
        printf("Tipo: %d\n", message.type);
        printf("Remetente: %d\n", message.sender);
        printf("Destinatario: %d\n", message.receiver);
        printf("Payload: %s\n", message.payload);
        printf("----------------------\n");
    }

    printf("----------------------\n\n");
}

// funcao de limpar os roteadores
void clear_routers()
{
    for (int i = 0; i < QTY_ROUTERS; i++)
    {
        if (router_id - 1 != i)
        {
            neighbors[i].id = -1;
            neighbors[i].cost = -1;
            neighbors[i].last_time_seen = 0;
        }
        else
        {
            neighbors[i].id = router_id;
            neighbors[i].cost = 0;
            neighbors[i].last_time_seen = 0;
        }
    }
}

// funcao de leitura dos arquivos de configuracao
int read_configs()
{
    printf("Configurando o roteador....\n");
    FILE *arquivo;
    int number_line = 1;
    char line[50];
    clear_routers();

    // iniciando a leitura do arquivo de enlaces.config
    arquivo = fopen(PATH_CONFIG_ENLACES, "r");
    if (arquivo == NULL)
    {
        printf("Erro ao abrir o arquivo %s\n", PATH_CONFIG_ENLACES);
        return 1;
    }

    while (fgets(line, sizeof(line), arquivo) != NULL)
    {
        int router1, router2, cost;
        int qty_values = sscanf(line, "%d %d %d", &router1, &router2, &cost);

        if (qty_values == 3)
        {
            if (router1 == router_id)
            {
                neighbors[router2 - 1].cost = cost;
            }
            else if (router2 == router_id)
            {
                neighbors[router1 - 1].cost = cost;
            }
        }
        else
        {
            printf("Caminhos: Linha %d mal formatada ou vazia. Ignorando-a.\n", number_line);
        }

        number_line++;
    }

    fclose(arquivo);

    // iniciando a leitura do arquivo roteador.config
    arquivo = fopen(PATH_CONFIG_ROTERS, "r");
    if (arquivo == NULL)
    {
        printf("Erro ao abrir o arquivo %s\n", PATH_CONFIG_ROTERS);
        return 1;
    }

    number_line = 1;

    while (fgets(line, sizeof(line), arquivo) != NULL)
    {
        int router, PORT;
        char ip[20];
        int qty_values = sscanf(line, "%d %d %19s", &router, &PORT, ip);

        if (qty_values == 3)
        {
            if (router == router_id)
            {
                port = PORT;
                strcpy(server, ip);
            }

            /// NOVA MANEIRA DE FAZER TESTANDO
            neighbors[router - 1].id = router;
            strcpy(neighbors[router - 1].ip, ip);
            neighbors[router - 1].port = PORT;
            neighbors[router - 1].last_time_seen = 0;
        }
        else
        {
            printf("Roteadores: Linha %d mal formatada ou vazia. Ignorando-a.\n", number_line);
        }

        number_line++;
    }

    fclose(arquivo);

    printf("Configurado com sucesso\n");
    printf("--------------------------\n");
    return 0;
}

// funcao de tratamento de erros
void die(const char *s)
{
    perror(s);
    exit(1);
}

void send_neighbors_to_control_package()
{
    char message_payload[PAYLOAD_SIZE] = "";

    for (int i = 0; i < QTY_ROUTERS; i++)
    {
        char buffer[100];
        sprintf(buffer, "%d:%d;", routing_table[i].destination, routing_table[i].cost);

        if (strlen(message_payload) + strlen(buffer) < PAYLOAD_SIZE)
        {
            strcat(message_payload, buffer);
        }
        else
        {
            printf("ERRO: payload estourou!\n");
            break;
        }
    }

    for (int i = 0; i < QTY_ROUTERS; i++)
    {
        if (neighbors[i].id != router_id && neighbors[i].cost != -1 && neighbors[i].id != -1)
        {
            Package pkg;

            pkg.type = CONTROL;
            pkg.sender = router_id;
            pkg.receiver = neighbors[i].id;

            strcpy(pkg.payload, message_payload);

            add_to_outbound_queue(pkg);
        }
    }
}

/*
Inicializa a tabela de roteamento para cada destino possível
com custo -1 e proximo roteador -1, depois atualiza com o valor conhecido de seus vizinhos
Já o vetor recebido inicializa tudo com -1, menos o do proprio roteador
*/
void initialize_routing_tables()
{

    for (int i = 0; i < QTY_ROUTERS; i++)
    {
        routing_table[i].destination = i + 1;
        routing_table[i].cost = -1;
        routing_table[i].next_router = -1;
    }

    routing_table[router_id - 1].cost = 0;
    routing_table[router_id - 1].next_router = router_id;

    for (int i = 0; i < QTY_ROUTERS; i++)
    {
        if (neighbors[i].cost > 0)
        {

            routing_table[i].cost = neighbors[i].cost;
            routing_table[i].next_router = i + 1;
        }
    }

    for (int i = 0; i < QTY_ROUTERS; i++)
    {
        for (int j = 0; j < QTY_ROUTERS; j++)
        {
            last_vectors[i][j] = -1;
        }
    }

    for (int i = 0; i < QTY_ROUTERS; i++)
    {
        last_vectors[router_id - 1][i] = routing_table[i].cost;
    }
}

/**
 * @todo
 */
void update_routing_table()
{
    pthread_mutex_lock(&routers_mutex);

    int change_table = 0;

    for (int i = 0; i < QTY_ROUTERS; i++)
    {
        if (i == router_id - 1 || neighbors[i].cost == -1)
        {
            continue;
        }

        for (int j = 0; j < QTY_ROUTERS; j++)
        {
            int lower_cost = routing_table[j].cost;
            int current_cost = -1;
            if (last_vectors[i][j] > 0 && neighbors[i].cost > 0)
            {
                current_cost = last_vectors[i][j] + neighbors[i].cost;
            }

            if ((current_cost < lower_cost || lower_cost == -1) && current_cost != 1)
            {
                change_table = 1;
                routing_table[j].cost = current_cost;
            }
        }
    }

    pthread_mutex_unlock(&routers_mutex);

    if (change_table)
    {
        // Aqui mudou a tabela tem q mandar o vetor de novo pros vizinhos
    }
}

/**
 * Peguei do gpt pq queria ver como tinha ficado as tabelas, depois da pra apagar
 * @brief Imprime a tabela de roteamento atual e a matriz de vetores de distância
 * recebidos de forma formatada e segura para threads.
 */
void print_tables()
{
    // Trava o mutex do console para garantir que a impressão não seja interrompida
    pthread_mutex_lock(&console_mutex);

    printf("\n\n############################################################\n");
    printf("###               VISUALIZACAO DAS TABELAS (Roteador %d)    ###\n", router_id);
    printf("############################################################\n\n");

    // --- Imprimindo a Tabela de Roteamento Principal ---
    printf("=== Tabela de Roteamento ATUAL ===\n");
    printf("+-------------+---------+---------------+\n");
    printf("|  Destino    |  Custo  | Proximo Salto |\n");
    printf("+-------------+---------+---------------+\n");

    for (int i = 0; i < QTY_ROUTERS; i++)
    {
        printf("|      %2d     |", routing_table[i].destination);

        // Imprime o custo de forma amigável (mostra 'inf' para infinito)
        if (routing_table[i].cost == -1)
        {
            printf("   inf   |");
        }
        else
        {
            printf("   %3d   |", routing_table[i].cost);
        }

        // Imprime o próximo salto (mostra '-' para indefinido)
        if (routing_table[i].next_router == -1)
        {
            printf("       -       |");
        }
        else
        {
            printf("      %2d       |", routing_table[i].next_router);
        }

        // Adiciona um comentário para a rota até si mesmo
        if (routing_table[i].destination == router_id)
        {
            printf(" <- (Este roteador)");
        }
        printf("\n");
    }
    printf("+-------------+---------+---------------+\n\n\n");

    // --- Imprimindo a Matriz de Vetores Recebidos ---
    printf("=== Matriz de Vetores de Distancia Recebidos (last_vectors) ===\n");
    printf("         (Visao de Custo para cada Destino Dst)\n");

    // Cabeçalho da matriz (Destinos)
    printf("De \\ Dst |");
    for (int j = 0; j < QTY_ROUTERS; j++)
    {
        printf("  %2d  |", j + 1);
    }
    printf("\n---------+");
    for (int j = 0; j < QTY_ROUTERS; j++)
    {
        printf("------+");
    }
    printf("\n");

    // Corpo da matriz (um roteador por linha)
    for (int i = 0; i < QTY_ROUTERS; i++)
    {
        // Só imprime a linha se for um vizinho ou o próprio roteador
        if (neighbors[i].cost != -1)
        {
            printf(" Rota %2d |", neighbors[i].id); // Cabeçalho da linha (Quem enviou o vetor)
            for (int j = 0; j < QTY_ROUTERS; j++)
            {
                // Imprime o custo que o roteador 'i+1' anunciou para o destino 'j+1'
                int cost = last_vectors[i][j];
                if (cost == -1)
                {
                    printf("  inf |");
                }
                else
                {
                    printf("  %3d |", cost);
                }
            }
            if (neighbors[i].id == router_id)
            {
                printf(" <- (Meu proprio vetor)");
            }
            printf("\n");
        }
    }
    printf("------------------------------------------------------------------------\n\n");

    // Libera o mutex do console
    pthread_mutex_unlock(&console_mutex);
}
