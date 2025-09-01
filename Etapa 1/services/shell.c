#include <stdio.h>
#include <string.h>
#include "common.h"

void* run_shell(void* arg) {
    char buffer[100];
    printf("Shell iniciado. Digite 'status' ou 'sair'.\n");

    while(1) {
        printf("router-%d > ", routerId); // Acessa a variável global routerId
        fgets(buffer, 100, stdin);
        buffer[strcspn(buffer, "\n")] = 0;

        if (strcmp(buffer, "sair") == 0) {
            // Lógica para encerrar o programa (usando mutex, como no exemplo anterior)
            printf("Encerrando...\n");
            break;
        } else if (strcmp(buffer, "status") == 0) {
            // Acessa as variáveis globais para mostrar o status
            printf("--- Status do Roteador ---\n");
            printf("ID: %d\n", routerId);
            printf("IP: %s\n", server);
            printf("Porta: %d\n", port);
            printf("--------------------------\n");
        }
    }
    return NULL;
}