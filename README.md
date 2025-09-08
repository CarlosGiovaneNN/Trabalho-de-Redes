# Trabalho de Redes de Computadores – 2025.2  
**Universidade Federal da Fronteira Sul – Campus Chapecó, SC**  
**Curso:** Ciência da Computação  
**Disciplina:** Redes de Computadores

## Descrição Geral  
Este trabalho tem como objetivo praticar **programação com sockets** e a **simulação de um protocolo de roteamento** em redes.  
Cada processo representará um roteador (nó da rede), que troca pacotes de roteamento via **sockets UDP**.  

Na versão final, os nós executarão o **algoritmo Bellman-Ford distribuído** para calcular suas tabelas de roteamento.  

### Observações
- Implementação em **linguagem C**  
- Plataforma **Linux**  
- Uso exclusivo de **sockets UDP**  
- Cada nó deve executar como um **processo multithread**  

## Funcionalidades
- Simulação de roteadores que se comunicam entre si via **UDP**.  
- Configuração de rede através dos arquivos:
  - `roteador.config` (IP, porta e ID do roteador)  
  - `enlaces.config` (IDRoteador, IDRoteadorVizinho, custo)  
- Roteadores trocam informações periodicamente até a convergência da tabela de rotas.  
- Cada atualização deve imprimir no console a **tabela de roteamento com timestamp**.  
- **Envio de mensagens de até 140 caracteres** entre quaisquer roteadores, roteadas conforme o algoritmo de Bellman-Ford.  

---

## Estrutura do Projeto

### Threads
- **receiver:** recebe mensagens dos roteadores vizinhos, e aloca-os na fila de entrada (inbound).  
- **sender:** pega os pacotes prontos da fila de sáida (outbound) e envia para os vizinhos.  
- **handler:** processa mensagens (controle via Bellman-Ford ou dados).  
- **shell:** interface com o usuário (menu de operações).  

### Estruturas de dados
- **Mensagens de controle e dados:** tipo, origem, destino, payload.  
- **Filas de entrada e saída** (uso de mutex para sincronização) e semáfotros para evitar espera ociosa.  
- **Tabela de roteamento** baseada em vetores distância.  

---

## 📂 Arquivos de Configuração  

### `roteador.config`
Formato:  
```
ID   PORTA   IP
```
Exemplo:  
```
1   25001   127.0.0.1
2   25002   127.0.0.1
3   25003   127.0.0.1
4   25004   127.0.0.1
```

### `enlaces.config`
Formato:  
```
ID1   ID2   CUSTO
```
Exemplo:  
```
1   2   3
2   3   2
2   4   10
3   4   3
```

---

## Como Usar

1.  **Clone o repositório:**

    ```bash
    git clone https://github.com/CarlosGiovaneNN/Trabalho-de-Redes.git
    ```

2.  **Navegue até o diretório do projeto:**

    ```bash
    cd Trabalho-de-Redes
    ````

3.  **Inicialize as configurações:**
   - Dentro do arquivo common.h defina a quantidade de roteadores na variável QTY_ROUTERS
   - Insira a topologia da rede no arquivo enlaces.config e as configurações do roteador no roteador.config
   OBS: Certifique-se de os arquvios estarem no formato exemplificado acima e que os IDs dos roteadores estejam no intervalo [1, QTY_ROUTERS]

4.  **Compile o programa:**
    Lembre-se que o programa só vai funcionar em um ambiente linux

    ```bash
    gcc *.c services/*.c -o router
    ```

5.  **Execute o programa:**
    IDRoteador deve ser um inteiro de 1 a 10
    ```bash
    ./router IDRoteador
    ```

## Autores

- **Marco Antonio Duz** Mátricula: 2311100006

- **Carlos Giovane Neu Nogueira** Mátricula: 2311100010