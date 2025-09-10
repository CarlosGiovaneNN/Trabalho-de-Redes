#include "common.h"
#include "../helper.h"

int showMenu();
void exitRouter();
void printStatus();
void sendPackage();
void showNeighbors();
void configuration();

void *run_shell(void *arg)
{
    printf("Shell iniciado.\n\n----------------------\n");

    while (1)
    {
        int buffer = showMenu();

        pthread_mutex_lock(&console_mutex);

        if (buffer == 0)
        {
            exitRouter();
            pthread_mutex_unlock(&console_mutex);
            break;
        }
        else if (buffer == 4)
        {
            configuration();
        }
        else if (buffer == 3)
        {
            showNeighbors();
        }
        else if (buffer == 2)
        {
            printStatus();
        }
        else if (buffer == 1)
        {
            sendPackage();
        }

        pthread_mutex_unlock(&console_mutex);
        usleep(1000);
    }

    return NULL;
}

int showMenu()
{
    char buffer[100];

    printf("\nDigite: \n1 - Enviar pacote.\n2 - Exibir status. \n3 - Mostrar vizinhos. \n4 - Configurar roteador.\n0 - Sair.\n\n");
    printf("router %d -> ", routerId);
    scanf("%s", buffer);

    return atoi(buffer);
}

void exitRouter()
{
    pthread_cancel(thread_receiver);
    pthread_cancel(thread_sender);
    pthread_cancel(thread_handler);

    printf("Encerrando...\n");
}

void printStatus()
{
    printf("\n--- Status do Roteador ---\n");
    printf("ID: %d\n", routerId);
    printf("IP: %s\n", server);
    printf("Porta: %d\n", port);
    printf("--------------------------\n\n");
}

void sendPackage()
{
    char type[10], sendTo[100], payload[140];

    printf("\nDigite o numero do roteador que deseja enviar o pacote:\n");
    for (int i = 0; i < QTY_ROUTERS; i++)
    {
        if (neighbors[i].id != routerId && neighbors[i].id != -1)
        {
            printf("%d - %s:%d ", neighbors[i].id, neighbors[i].ip, neighbors[i].port);
            if (neighbors[i].cost == -1)
            {
                printf("(Indisponivel) ");
            }
            printf("\n");
        }
    }
    printf("\n->");
    scanf(" %99s", sendTo);

    int sendToId = atoi(sendTo);

    if ((sendToId <= 0 || sendToId == routerId || sendToId > QTY_ROUTERS))
    {
        printf("Roteador inválido!\n\n");
        return;
    }

    Router sendToRouter = neighbors[sendToId - 1];

    if (sendToRouter.id == -1)
    {
        printf("Roteador não encontrado!\n\n");
        return;
    }
    else if (sendToRouter.cost == -1)
    {
        printf("Este roteador não é um vizinho!\n\n");
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
    package.sender = routerId;
    package.receiver = sendToId;
    strcpy(package.payload, payload);

    addToOutboundQueue(package);

    usleep(1000);
}

void showNeighbors()
{
    for (int i = 0; i < QTY_ROUTERS; i++)
    {
        if (neighbors[i].id != routerId && neighbors[i].id != -1 && neighbors[i].cost != -1)
        {
            printf("Vizinho: %d - %s:%d, custo: %d\n", neighbors[i].id, neighbors[i].ip, neighbors[i].port, neighbors[i].cost);
        }
    }
    printf("\n----------------------\n");
}

void configurateDistance()
{
    char buffer[100];
    printf("Digite o numero do roteador que deseja configurar a distancia:\n");
    for (int i = 0; i < QTY_ROUTERS; i++)
    {
        if (neighbors[i].id != routerId && neighbors[i].id != -1)
        {
            printf("%d - %s:%d, custo: %d \n", neighbors[i].id, neighbors[i].ip, neighbors[i].port, neighbors[i].cost == -1 ? 0 : neighbors[i].cost);
        }
    }

    printf("\n->");
    scanf(" %99s", buffer);

    int neighborId = atoi(buffer);

    if ((neighborId <= 0 || neighborId == routerId || neighborId > QTY_ROUTERS))
    {
        printf("Roteador inválido!\n\n");
        return;
    }

    Router sendToRouter = neighbors[neighborId - 1];

    if (sendToRouter.id == -1)
    {
        printf("Roteador não encontrado!\n\n");
        return;
    }
    else if (sendToRouter.cost == -1)
    {
        printf("Este roteador não é um vizinho!\n\n");
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

    neighbors[neighborId - 1].cost = cost;

    Package sendToRouterPackage;

    sendNeighborsToControlPackage();
}

void configurateTime()
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

    timeToControlPackage = atoi(buffer);
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
        configurateDistance();
    }
    else if (atoi(buffer) == 2)
    {
        configurateTime();
    }
    else
    {
        printf("Configuracao invalida!\n\n");
    }
}