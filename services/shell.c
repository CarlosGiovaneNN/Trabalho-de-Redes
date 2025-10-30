#include "../helper/helper.h"
#include "../router.h"

int show_menu();
void exit_router();
void print_status();
void send_package();
void show_neighbors();
void configuration();

void *run_shell(void *arg)
{
    printf("Shell iniciado.\n\n----------------------\n");

    while (1)
    {
        int buffer = show_menu();

        pthread_mutex_lock(&console_mutex);

        if (buffer == 0)
        {
            exit_router();
            pthread_mutex_unlock(&console_mutex);
            break;
        }
        else if (buffer == 5)
        {
            print_tables();
        }
        else if (buffer == 4)
        {
            configuration();
        }
        else if (buffer == 3)
        {
            show_neighbors();
        }
        else if (buffer == 2)
        {
            print_status();
        }
        else if (buffer == 1)
        {
            send_package();
        }

        pthread_mutex_unlock(&console_mutex);
        usleep(1000);
    }

    return NULL;
}

int show_menu()
{
    char buffer[100];

    printf("\nDigite:\n");

    printf("1 - Enviar pacote.\n");
    printf("2 - Exibir status.\n");
    printf("3 - Mostrar vizinhos.\n");
    printf("4 - Configurar roteador.\n");
    printf("5 - Exibir tabela de roteamento.\n");

    printf("0 - Sair.\n\n");
    printf("router %d -> ", router_id);
    scanf("%s", buffer);

    return atoi(buffer);
}

void exit_router()
{
    pthread_cancel(thread_receiver);
    pthread_cancel(thread_sender);
    pthread_cancel(thread_handler);
    pthread_cancel(thread_router_controller);

    printf("Encerrando...\n");
}

void print_status()
{
    printf("\n--- Status do Roteador ---\n");
    printf("ID: %d\n", router_id);
    printf("IP: %s\n", server);
    printf("Porta: %d\n", port);
    printf("--------------------------\n\n");
}

void send_package()
{
    char type[10], send_to[100], payload[140];

    printf("\nDigite o numero do roteador que deseja enviar o pacote:\n");
    for (int i = 0; i < QTY_ROUTERS; i++)
    {
        if (neighbors[i].id != router_id && neighbors[i].id != -1)
        {
            printf("%d - %s:%d ", neighbors[i].id, neighbors[i].ip, neighbors[i].port);
            if (routing_table[i].cost == -1)
            {
                printf("(Indisponivel) ");
            }
            printf("\n");
        }
    }
    printf("\n->");
    scanf(" %99s", send_to);

    int send_to_id = atoi(send_to);

    if ((send_to_id <= 0 || send_to_id == router_id || send_to_id > QTY_ROUTERS))
    {
        printf("Roteador inválido!\n\n");
        return;
    }

    Router send_to_router = neighbors[send_to_id - 1];

    if (send_to_router.id == -1)
    {
        printf("Roteador não encontrado!\n\n");
        return;
    }
    else if (routing_table[send_to_id - 1].cost == -1)
    {
        printf("Este roteador não é alcançavel!\n\n");
        return;
    }

    printf("\nDigite o tipo do pacote: \n0 - Controle\n1 - Dados\n\n->");
    scanf(" %9s", type);
    // Por algum motivo se voce colocar algo com epaço aqui nesse scanf ele encerra o programa

    if (!strcmp(type, "0") == 0 && !strcmp(type, "1") == 0)
    {
        printf("Tipo inválido!\n\n");
        return;
    }

    printf("\nDigite o payload: ");
    printf("\n->");
    scanf(" %[^\n]", payload);

    Package package;

    package.type = atoi(type);
    package.sender = router_id;
    package.receiver = send_to_id;
    strcpy(package.payload, payload);

    add_to_outbound_queue(package);

    usleep(1000);
}

void show_neighbors()
{
    for (int i = 0; i < QTY_ROUTERS; i++)
    {
        if (neighbors[i].id != router_id && neighbors[i].id != -1 && neighbors[i].cost != -1)
        {
            printf("Vizinho: %d - %s:%d, custo: %d\n", neighbors[i].id, neighbors[i].ip, neighbors[i].port,
                   neighbors[i].cost);
        }
    }
    printf("\n----------------------\n");
}

void configurate_distance()
{
    char buffer[100];
    printf("Digite o numero do roteador que deseja configurar a distancia:\n");
    for (int i = 0; i < QTY_ROUTERS; i++)
    {
        if (neighbors[i].id != router_id && neighbors[i].id != -1)
        {
            printf("%d - %s:%d, custo: %d \n", neighbors[i].id, neighbors[i].ip, neighbors[i].port,
                   neighbors[i].cost == -1 ? 0 : neighbors[i].cost);
        }
    }

    printf("\n->");
    scanf(" %99s", buffer);

    int neighbor_id = atoi(buffer);

    if ((neighbor_id <= 0 || neighbor_id == router_id || neighbor_id > QTY_ROUTERS))
    {
        printf("Roteador inválido!\n\n");
        return;
    }

    Router send_to_router = neighbors[neighbor_id - 1];

    if (send_to_router.id == -1)
    {
        printf("Roteador não encontrado!\n\n");
        return;
    }

    printf("\nDigite o custo da distancia: ");
    printf("\n->");
    scanf(" %99s", buffer);

    int cost = atoi(buffer);

    if (cost < 0)
    {
        printf("Custo inválido!\n\n");
        return;
    }

    neighbors[neighbor_id - 1].cost = cost;
    routing_table[neighbor_id - 1].cost = cost;

    Package send_to_router_package;

    send_neighbors_to_control_package();
}

void configurate_time()
{
    char buffer[100];
    printf("Tempo de envio de pacotes de controle(em segundos): ");
    printf("\n->");
    scanf(" %99s", buffer);

    if (atoi(buffer) < 0)
    {
        printf("Tempo inválido!\n\n");
        return;
    }

    time_to_control_package = atoi(buffer);
}

void configuration()
{
    printf("Digite o numero da configuracao: \n");
    printf("1 - Configurar distancias dos roteadores\n");
    printf("2 - Configurar tempo de envio de pacotes de controle\n");

    char buffer[100];
    printf("\n->");
    scanf(" %99s", buffer);

    if (atoi(buffer) == 1)
    {
        configurate_distance();
    }
    else if (atoi(buffer) == 2)
    {
        configurate_time();
    }
    else
    {
        printf("Configuracao invalida!\n\n");
    }
}