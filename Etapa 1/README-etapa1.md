# Etapa 01 – Trabalho de Redes de Computadores (2025.2)

## Funcionalidades
- Configuração inicial do roteador a partir de `roteador.config` e `enlaces.config`.
- Criação de processo multithread com as threads:
  - `receiver`
  - `sender`
  - `packet_handler`
  - `terminal`
- Envio e recepção de mensagens simples entre vizinhos.
- Exibição no console das mensagens recebidas e enviadas.

## Compilação
```bash
gcc -o roteador etapa1.c -lpthread
```

## Execução
```bash
./roteador <ID_ROTEADOR>
```

Exemplo:
```bash
./roteador 1
```

## Exemplo de arquivos de configuração
`roteador.config`:
```
1 25001 127.0.0.1
2 25002 127.0.0.1
```

`enlaces.config`:
```
1 2 3
```
