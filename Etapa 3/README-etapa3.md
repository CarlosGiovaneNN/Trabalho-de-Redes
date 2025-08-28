# Etapa 03 – Trabalho de Redes de Computadores (2025.2)

## Funcionalidades adicionais
- Implementação do algoritmo **Bellman-Ford distribuído**.
- Atualização automática das tabelas de roteamento sempre que houver mudanças.
- Encaminhamento de **mensagens de dados (até 100 caracteres)** entre quaisquer roteadores da rede.
- Exibição do caminho percorrido no console:
  - "Roteador X encaminhando mensagem com #seq N para destino Y".

## Compilação
```bash
gcc -o roteador etapa3.c -lpthread
```

## Execução
```bash
./roteador <ID_ROTEADOR>
```

## Exemplo de envio de mensagem
No terminal do roteador 1:
```
Digite destino e mensagem: 3 Olá, mundo!
```

Saída no console:
```
Roteador 1 encaminhando mensagem para 2
Roteador 2 encaminhando mensagem para 3
Roteador 3 recebeu mensagem: "Olá, mundo!"
```
