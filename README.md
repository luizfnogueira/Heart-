# 💜 HEART! 💜

Um jogo de ação e estratégia desenvolvido em C com a biblioteca Raylib, utilizando estruturas de dados avançadas e integração com IA.

## 📝 Descrição

HEART é um jogo desenvolvido para a disciplina de Análise e Estrutura de Dados, onde o jogador controla um coração que deve navegar por um campo de obstáculos, enfrentar chefes e responder charadas geradas por Inteligência Artificial para progredir. O jogo implementa diversas estruturas de dados como listas duplamente encadeadas, listas circulares e pilhas, demonstrando aplicações práticas desses conceitos em um ambiente interativo.

### Público Alvo
Estudantes de ciência da computação, entusiastas de programação e jogadores casuais interessados em ver como estruturas de dados e algoritmos podem ser aplicados em um jogo real.

### Narrativa
Em um mundo onde a luz se apagou, você é a última centelha de um coração que já foi cheio de vida. Preso em uma dimensão entre o vazio e a esperança, você enfrenta os ecos sombrios dos medos e arrependimentos de uma alma perdida. Cada obstáculo superado e cada charada resolvida é uma luta para recuperar fragmentos de sentimentos e memórias esquecidas.

## 🎮 Mecânicas Principais

- **Movimentação**: Contr ole preciso do coração com as teclas WASD em uma área de jogo delimitada.
- **Obstáculos Dinâmicos**: 
- **Obstáculos Brancos**: Gerenciados por uma lista circular, causam dano ao coração em caso de colisão.
- **Gemini API**: Mensagens positivas geradas por IA.
- **Sistema de Chefes**: Batalhas contra chefes com padrões de ataque únicos ao final de cada fase.
- **Sistema de Pontuação**: Registra e classifica as pontuações usando algoritmo Bubble Sort.

## 🎯 Progresso do Jogo

O jogo possui três fases com dificuldade crescente:
1. **Fase 1**: Introdução aos obstáculos básicos e mecânicas de movimento.
2. **Fase 2**: Aumento na velocidade e quantidade de obstáculos, introdução de charadas mais complexas.
3. **Fase 3**: Desafio máximo com obstáculos que perseguem o jogador e chefe final.

## 🧠 Estruturas de Dados Implementadas

### Lista Duplamente Encadeada
Implementada para gerenciar as charadas do jogo, permitindo navegação bidirecional entre os elementos. Cada nó contém uma charada e ponteiros para os nós anterior e próximo, facilitando a busca e manipulação dos desafios apresentados ao jogador.

```c
typedef struct NodoCharada {
    Charada charada;
    struct NodoCharada* proximo;
    struct NodoCharada* anterior; // Ponteiro para o nó anterior
} NodoCharada;
```

**Funções implementadas**: `inicializarListaCharadas`, `listaCharadasVazia`, `inserirCharada`, `removerCharada`, `liberarListaCharadas`, `buscarCharada`

### Lista Circular
Utilizada para gerenciar os obstáculos no jogo, criando um fluxo contínuo e eficiente. A estrutura permite que o último elemento aponte para o primeiro, formando um ciclo, ideal para processar elementos de forma cíclica.

```c
typedef struct NodoObstaculo {
    Obstaculo obstaculo;
    struct NodoObstaculo* proximo;
} NodoObstaculo;

typedef struct {
    NodoObstaculo* ultimo;  // Aponta para o último nó, cujo próximo é o primeiro
    int tamanho;
} FilaObstaculos;
```

**Funções implementadas**: `inicializarFilaObstaculos`, `filaObstaculosVazia`, `enfileirarObstaculo`, `desenfileirarObstaculo`, `percorrerObstaculos`

### Pilha
Implementada para gerenciar os números de dano que aparecem quando o jogador é atingido, seguindo o princípio LIFO (Last In, First Out). Garante que os números mais recentes sejam processados primeiro, criando um efeito visual de sobreposição.

```c
typedef struct {
    NumeroDano itens[MAX_NUMEROS_DANO];
    int topo;
} PilhaNumerosDano;
```

**Funções implementadas**: `inicializarPilhaNumerosDano`, `pilhaNumerosDanoCheia`, `pilhaNumerosDanoVazia`, `empilharNumeroDano`, `desempilharNumeroDano`

## 🔄 Algoritmos de Ordenação

### Lista Encadeada
Utilizada para armazenar e gerenciar as pontuações dos jogadores no ranking.

**Justificativa**: A lista encadeada foi escolhida por sua flexibilidade na inserção e remoção de elementos, permitindo um gerenciamento eficiente das pontuações sem necessidade de realocação de memória, como seria o caso em um array.

## 🔄 Algoritmo de Ordenação

### Bubble Sort
Implementado na função `ordenarPontuacoesBubbleSort()` no arquivo `pontuacao.c`, este algoritmo é utilizado para ordenar as pontuações no ranking em ordem decrescente (da maior para a menor).

**Como funciona**: O algoritmo percorre a lista de pontuações repetidamente, comparando pares adjacentes e trocando-os se estiverem na ordem errada. Este processo é repetido até que nenhuma troca seja necessária, indicando que a lista está ordenada.

**Implementação**: Nossa implementação do Bubble Sort foi adaptada para trabalhar com uma lista encadeada, realizando trocas de nós em vez de simples trocas de valores. Isso adiciona um nível extra de complexidade, mas mantém a eficiência de memória da estrutura de lista encadeada.

**Justificativa**: O Bubble Sort foi escolhido por sua simplicidade de implementação e eficiência para pequenas quantidades de dados, como é o caso do ranking de pontuações. Embora não seja o algoritmo mais eficiente para grandes conjuntos de dados (complexidade O(n²)), para a escala deste jogo, ele é adequado e facilmente compreensível.

## 🤖 Integração com API de Inteligência Artificial

### API Gemini do Google
O jogo integra-se com a API Gemini do Google para gerar charadas dinâmicas durante o gameplay. Esta integração é implementada no arquivo `charada.c` e representa um componente central da experiência de jogo:

```c
bool gerarCharadaGemini(int fase, char* pergunta, char* respostaCorreta, char* alternativa1, char* alternativa2) {
    // Código para gerar charadas usando a API Gemini
}
```

**Funcionalidades implementadas**:
- Geração de charadas personalizadas com base na fase atual do jogo
- Criação de alternativas de resposta para cada charada
- Adaptação do nível de dificuldade das charadas conforme o progresso do jogador
- Processamento de respostas JSON complexas da API
- Sistema de fallback para charadas pré-definidas em caso de falha na API

A implementação utiliza a biblioteca libcurl para fazer requisições HTTP à API e a biblioteca json-c para processar as respostas JSON recebidas. As charadas geradas pela IA são armazenadas na lista duplamente encadeada para uso durante o jogo.

### IA para Comportamento de Inimigos
Na fase final do jogo, os obstáculos e chefes utilizam algoritmos de IA para perseguir o jogador, implementados no arquivo `boss.c`. Os padrões de movimento são calculados dinamicamente com base na posição do coração, criando uma experiência desafiadora e adaptativa.

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

1. Certifique-se de ter as bibliotecas necessárias instaladas:
   - Raylib (para a interface gráfica)
   - libcurl (para integração com API)
   - json-c (para processamento de JSON)

2. Compile o projeto usando:
   ```
   gcc -Wall -std=c99 -I./raylib/src main.c menu.c jogo.c pontuacao.c boss.c charada.c estruturas.c algoritmos.c -o heart ./raylib/src/libraylib.a -lGL -lm -lpthread -ldl -lrt -lX11 -lcurl -ljson-c
   ```

3. Execute o jogo:
   ```
   ./heart
   ```

4. Alternativamente, use o Makefile incluído no projeto:
   ```
   make
   ./heart
   ```

---

*Este projeto foi desenvolvido como parte da disciplina de Análise e Estrutura de Dados.*