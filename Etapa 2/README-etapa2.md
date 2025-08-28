# Etapa 02 – Trabalho de Redes de Computadores (2025.2)

## Funcionalidades adicionais
- Definição e armazenamento dos **vetores distância**.
- Definição da **tabela de roteamento**.
- Envio periódico do vetor distância para os vizinhos.
- Exibição dos vetores distância enviados e recebidos no console.

## Compilação
```bash
gcc -o roteador etapa2.c -lpthread
```

## Execução
```bash
./roteador <ID_ROTEADOR>
```

## Novos testes recomendados
- Alterar os custos em `enlaces.config` e observar como os vetores distância mudam.
- Derrubar (matar o processo) de um roteador e ver como a rede se adapta.
