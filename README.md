# 💜 HEART! 💜

## 📋 Requisitos da Atividade

Para o desenvolvimento deste projeto, foram seguidos os seguintes requisitos obrigatórios:

- O programa foi desenvolvido integralmente em linguagem **C**.
- O jogo apresenta um **menu de opções interativas** para o usuário.
- Utiliza **estruturas de dados avançadas** (lista circular, pilha e lista encadeada) que fazem parte da lógica central do jogo, não apenas para armazenamento, mas para o funcionamento dinâmico dos obstáculos, pontuação e efeitos visuais.
- Implementa **(ao menos) um algoritmo de ordenação** apresentado em sala de aula (Bubble Sort), aplicado diretamente ao ranking de pontuações.
- Realiza **integração com uma API de Inteligência Artificial** (API Gemini do Google), de modo que as mensagens geradas influenciam diretamente a narrativa e atmosfera do jogo.
- Utiliza **(ao menos) 5 funções** que envolvem as estruturas de dados adotadas, como:
  - `inicializarFilaObstaculos`
  - `enfileirarObstaculo`
  - `desenfileirarObstaculo`
  - `percorrerObstaculos`
  - `inicializarPilhaNumerosDano`
  - `empilharNumeroDano`
  - `desempilharNumeroDano`
  - `ordenarPontuacoesBubbleSort`
- As principais funções do jogo estão especificadas nas seções de Estruturas de Dados e Algoritmos de Ordenação deste README.
- **Interface gráfica**: Embora não obrigatória, foi implementada utilizando a biblioteca Raylib, garantindo um bônus de +0,5 pt na média do MOD02.

---

## 📝 Descrição

HEART é um jogo desenvolvido para a disciplina de Análise e Estrutura de Dados, onde o jogador controla um coração que deve navegar por um campo de obstáculos, enfrentar chefes e receber mensagens geradas por Inteligência Artificial para progredir. O jogo implementa diversas estruturas de dados como listas circulares e pilhas, demonstrando aplicações práticas desses conceitos em um ambiente interativo.

### Público Alvo
Estudantes de ciência da computação, entusiastas de programação e jogadores casuais interessados em ver como estruturas de dados e algoritmos podem ser aplicados em um jogo real.

### Narrativa
Em um mundo onde a luz se apagou, você é a última centelha de um coração que já foi cheio de vida. Preso em uma dimensão entre o vazio e a esperança, você enfrenta os ecos sombrios dos medos e arrependimentos de uma alma perdida. Cada obstáculo superado é uma luta para recuperar fragmentos de sentimentos e memórias esquecidas. Mensagens poéticas e sombrias, geradas por IA, surgem durante a jornada, aprofundando a atmosfera do jogo.

## 🎮 Mecânicas Principais

- **Movimentação**: Controle preciso do coração com as teclas WASD em uma área de jogo delimitada.
- **Obstáculos Dinâmicos**: 
- **Obstáculos Brancos**: Gerenciados por uma lista circular, causam dano ao coração em caso de colisão.
- **Gemini API**: Mensagens poéticas e reflexivas geradas automaticamente por IA, exibidas durante a narrativa.
- **Sistema de Chefes**: Batalhas contra chefes com padrões de ataque únicos ao final de cada fase.
- **Sistema de Pontuação**: Registra e classifica as pontuações usando algoritmo Bubble Sort.

## 🎯 Progresso do Jogo

O jogo possui três fases com dificuldade crescente:
1. **Fase 1**: Introdução aos obstáculos básicos e mecânicas de movimento.
2. **Fase 2**: Aumento na velocidade e quantidade de obstáculos.
3. **Fase 3**: Desafio máximo com obstáculos que perseguem o jogador e chefe final.

## 🧠 Estruturas de Dados Utilizadas

O projeto HEART utiliza diversas estruturas de dados clássicas, cada uma aplicada diretamente à lógica central do jogo. Veja abaixo as principais estruturas, onde são usadas e seus códigos de referência:

### 1. Lista Circular de Obstáculos
- **Arquivo:** `estruturas.h` / `estruturas.c`
- **Código:**
  ```c
  typedef struct NodoObstaculo {
      Obstaculo obstaculo;
      struct NodoObstaculo* proximo;
  } NodoObstaculo;

  typedef struct {
      NodoObstaculo* ultimo;
      int tamanho;
  } FilaObstaculos;
  ```
- **Funções:** `inicializarFilaObstaculos`, `enfileirarObstaculo`, `desenfileirarObstaculo`, `percorrerObstaculos`
- **Uso:** Gerenciamento dinâmico dos obstáculos brancos na área de jogo.

### 2. Pilha de Números de Dano
- **Arquivo:** `estruturas.h` / `estruturas.c`
- **Código:**
  ```c
  typedef struct {
      NumeroDano itens[MAX_NUMEROS_DANO];
      int topo;
  } PilhaNumerosDano;
  ```
- **Funções:** `inicializarPilhaNumerosDano`, `empilharNumeroDano`, `desempilharNumeroDano`, `pilhaNumerosDanoCheia`, `pilhaNumerosDanoVazia`
- **Uso:** Exibição e controle dos números de dano que aparecem sobre o coração ao ser atingido.

### 3. Lista Duplamente Encadeada de Mensagens
- **Arquivo:** `estruturas.h` / `estruturas.c`
- **Código:**
  ```c
  typedef struct NodoMensagem {
      MensagemConforto mensagem;
      struct NodoMensagem* proximo;
      struct NodoMensagem* anterior;
  } NodoMensagem;

  typedef struct {
      NodoMensagem* inicio;
      NodoMensagem* fim;
      int tamanho;
  } ListaMensagens;
  ```
- **Funções:** `inicializarListaMensagens`, `inserirMensagem`, `removerMensagem`, `liberarListaMensagens`, `buscarMensagem`
- **Uso:** Gerenciamento das mensagens atmosféricas/poéticas exibidas durante o jogo.

### 4. Árvore Binária de Bosses
- **Arquivo:** `estruturas.h` / `estruturas.c`
- **Código:**
  ```c
  typedef struct NodoArvore {
      Boss boss;
      struct NodoArvore* esquerda;
      struct NodoArvore* direita;
  } NodoArvore;

  typedef struct {
      NodoArvore* raiz;
  } ArvoreBosses;
  ```
- **Funções:** `inicializarArvoreBosses`, `inserirBoss`, `buscarBoss`, `liberarArvoreBosses`, `percorrerEmOrdem`
- **Uso:** Organização e progressão dos chefes (bosses) do jogo por fase.

### 5. Lista Encadeada de Pontuações
- **Arquivo:** `pontuacao.h` / `pontuacao.c`
- **Código:**
  ```c
  typedef struct NoPontuacao {
      char nome[50];
      int pontuacao;
      struct NoPontuacao* proximo;
  } NoPontuacao;

  typedef struct ListaPontuacao {
      NoPontuacao* cabeca;
      int maiorPontuacao;
      char nomeMaiorPontuacao[50];
  } ListaPontuacao;
  ```
- **Funções:** `inicializarListaPontuacao`, `adicionarPontuacao`, `ordenarPontuacoesBubbleSort`, `salvarPontuacoes`, `carregarPontuacoes`, `liberarListaPontuacao`
- **Uso:** Armazenamento, ordenação e exibição do ranking de jogadores.

## 🔄 Algoritmos de Ordenação

### Bubble Sort
Implementado na função `ordenarPontuacoesBubbleSort()` no arquivo `pontuacao.c`, este algoritmo é utilizado para ordenar as pontuações no ranking em ordem decrescente (da maior para a menor).

**Como funciona**: O algoritmo percorre a lista de pontuações repetidamente, comparando pares adjacentes e trocando-os se estiverem na ordem errada. Este processo é repetido até que nenhuma troca seja necessária, indicando que a lista está ordenada.

**Implementação**: Nossa implementação do Bubble Sort foi adaptada para trabalhar com uma lista encadeada, realizando trocas de nós em vez de simples trocas de valores. Isso adiciona um nível extra de complexidade, mas mantém a eficiência de memória da estrutura de lista encadeada.

**Justificativa**: O Bubble Sort foi escolhido por sua simplicidade de implementação e eficiência para pequenas quantidades de dados, como é o caso do ranking de pontuações. Embora não seja o algoritmo mais eficiente para grandes conjuntos de dados (complexidade O(n²)), para a escala deste jogo, ele é adequado e facilmente compreensível.

## 🤖 Integração com API de Inteligência Artificial

### API Gemini do Google

O jogo integra-se com a **API Gemini do Google** para gerar **mensagens automáticas** durante a jogabilidade. Estas mensagens possuem um caráter **poético, sombrio e reflexivo**, sendo exibidas de forma dinâmica em momentos específicos da narrativa, contribuindo para a imersão do jogador no universo do jogo.

A integração com a IA é implementada no arquivo `gemini.c`, que:

- Realiza requisições HTTP usando a biblioteca `libcurl`
- Processa respostas JSON com a biblioteca `json-c`
- Exibe os textos gerados por IA diretamente na interface gráfica utilizando `raylib`

As mensagens geradas não dependem da interação do jogador e não funcionam como charadas ou perguntas. Elas compõem a atmosfera do jogo, funcionando como elementos narrativos automáticos — sem múltiplas escolhas, tentativas ou respostas.

### Finalidade da IA no Jogo

- Geração de mensagens sombrias e emocionais durante o jogo
- Integração transparente com a narrativa de cada fase
- Substituição total da antiga lógica de charadas interativas
- Experiência mais imersiva e cinematográfica

## 🛠️ Funcionalidades Principais

1. **Sistema de Fases**: O jogo possui três fases com dificuldade crescente.
2. **Sistema de Colisão**: Detecção precisa de colisões entre o coração e diferentes tipos de obstáculos.
3. **Sistema de Pontuação**: Armazenamento persistente de pontuações em arquivo.
4. **Ranking de Jogadores**: Exibição das melhores pontuações ordenadas.

## 🖥️ Interface Gráfica

O jogo utiliza a biblioteca Raylib para renderização gráfica, oferecendo uma experiência visual agradável com elementos coloridos e animações simples.

## 📚 Referências Bibliográficas

- Documentação da Raylib: [https://www.raylib.com/](https://www.raylib.com/)
- Cormen, T. H., Leiserson, C. E., Rivest, R. L., & Stein, C. (2009). Introduction to Algorithms (3rd ed.). MIT Press.

## 👨‍💻 Desenvolvido por

- Thiago Queiroz
- Lucas Rodrigues
- Luiz Nogueira

## 🔧 Compilação e Execução

1. Certifique-se de ter as bibliotecas necessárias instaladas no sistema:
   - **Raylib** (para a interface gráfica)
   - **libcurl** (para integração com API)
   - **json-c** (para processamento de JSON)

   No Ubuntu ou WSL (Windows Subsystem for Linux), instale com:

   ```bash
   sudo apt update
   sudo apt install libgl1-mesa-dev libx11-dev libxcursor-dev libxrandr-dev libxinerama-dev libxi-dev libcurl4-openssl-dev libjson-c-dev build-essential
   ```

3. Execute o jogo:
   ```
   make
   ./heart
   ```

---

*Este projeto foi desenvolvido como parte da disciplina de Análise e Estrutura de Dados.*
