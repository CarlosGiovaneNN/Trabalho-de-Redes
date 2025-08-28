# Trabalho de Redes de Computadores – 2025.2  
**Universidade Federal da Fronteira Sul – Campus Chapecó, SC**  
**Curso:** Ciência da Computação  
**Disciplina:** Redes de Computadores

## 📌 Descrição Geral  
Este trabalho tem como objetivo praticar **programação com sockets** e a **simulação de um protocolo de roteamento** em redes.  
Cada processo representará um roteador (nó da rede), que troca pacotes de roteamento via **sockets UDP**.  

Na versão final, os nós executarão o **algoritmo Bellman-Ford distribuído** para calcular suas tabelas de roteamento.  

### Observações
- Implementação em **linguagem C**  
- Plataforma **Linux**  
- Uso exclusivo de **sockets UDP**  
- Cada nó deve executar como um **processo multithread**  
- A submissão deve conter **todos os arquivos necessários** para compilação e execução, incluindo um **README**  

### Modalidade
- Individual ou em dupla  

---

## ⚙️ Funcionalidades
- Simulação de roteadores que se comunicam entre si via **UDP**.  
- Configuração de rede através dos arquivos:
  - `roteador.config` (IP, porta e ID do roteador)  
  - `enlaces.config` (custos dos enlaces entre roteadores)  
- Roteadores trocam informações periodicamente até a convergência da tabela de rotas.  
- Roteadores podem ser **ligados/desligados** dinamicamente.  
- Detecção do problema de **contagem ao infinito** (limitada por um valor máximo).  
- Cada atualização deve imprimir no console a **tabela de roteamento com timestamp**.  
- **Envio de mensagens de até 100 caracteres** entre quaisquer roteadores, roteadas conforme o algoritmo de Bellman-Ford.  

---

## 🏗️ Estrutura do Projeto

### Threads mínimas
- **receiver:** recebe mensagens dos roteadores vizinhos.  
- **sender:** envia mensagens para vizinhos.  
- **packet_handler:** processa mensagens (controle via Bellman-Ford ou dados).  
- **terminal:** interface com o usuário (menu de operações).  

### Estruturas de dados
- **Mensagens de controle e dados:** tipo, origem, destino, payload.  
- **Filas de entrada e saída** (uso de mutex para sincronização).  
- **Tabela de roteamento** baseada em vetores distância.  

---

## 📑 Etapas do Projeto  

### Etapa 01 – **30/09/2025**
- Implementação básica do roteador multithread.  
- Estruturas de mensagens, filas de entrada/saída.  
- Envio e recepção de mensagens simples entre vizinhos.  

### Etapa 02 – **19/10/2025**
- Definição de vetores distância e tabela de roteamento.  
- Envio periódico do vetor distância aos vizinhos.  
- Opção de visualizar os vetores recebidos.  

### Etapa 03 (Final) – **09/11/2025**
- Implementação completa do **Bellman-Ford distribuído**.  
- Atualização automática da tabela de rotas.  
- Encaminhamento de mensagens de dados entre quaisquer roteadores.  

### Apresentação final – **13 e 14/11/2025**

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

## 📜 Instruções de Submissão
- A cada etapa, entregar um **.zip** contendo:  
  - Código-fonte comentado  
  - Arquivos de configuração  
  - README com instruções de compilação e uso  

---

## 💡 Dicas
- Teste com diferentes cenários de topologia.  
- Ative/desative roteadores e observe as mudanças.  
- Compare os resultados no programa com simulações feitas no papel.  
- Não deixe para a última hora.  
