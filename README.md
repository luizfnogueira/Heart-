# 💜 HEART! 💜

Uma reimplementação do jogo Heart utilizando a biblioteca Raylib e estruturas de dados avançadas.

## 📝 Descrição

Este projeto é uma reimplementação do jogo Heart original, desenvolvido para a disciplina de Análise e Estrutura de Dados. O jogo consiste em controlar um coração que deve desviar de obstáculos para sobreviver, responder charadas geradas por IA e enfrentar desafios crescentes.

### Público Alvo
Estudantes e entusiastas de jogos que apreciam desafios de desviar de obstáculos com mecânicas simples mas envolventes, além de pessoas interessadas em ver aplicações práticas de estruturas de dados e integração com IA.

### Narrativa
Em um mundo onde a luz se apagou, você é a última centelha de um coração que já foi cheio de vida. Preso em uma dimensão entre o vazio e a esperança, você enfrenta os ecos sombrios dos medos e arrependimentos de uma alma perdida. Cada obstáculo superado é uma luta para recuperar fragmentos de sentimentos e memórias esquecidas.

## 🎮 Como Jogar

- **Movimentação**: Utilize as teclas W, A, S, D para mover o coração.
- **Obstáculos**:
  - **Obstáculos Brancos**: Desvie a todo custo! Colidir com eles causa dano ao coração.
  - **Obstáculos Roxos**: Podem ser atravessados sem dano, mas apenas se você permanecer imóvel ao passar por eles.
  - **Obstáculos Amarelos**: Colete-os para recuperar vida.
- **Charadas**: Responda corretamente às charadas geradas pela IA para avançar no jogo e ganhar bônus.
- **Chefes**: Enfrente chefes desafiadores ao final de cada fase.

## 🎯 Objetivo

O objetivo do jogo é sobreviver o máximo possível, acumulando pontos e avançando pelas três fases do jogo até derrotar o chefe final e restaurar completamente o coração.

## 🧠 Estruturas de Dados Implementadas

### Lista Duplamente Encadeada
Utilizada para armazenar e gerenciar as charadas do jogo. A estrutura permite navegação bidirecional entre as charadas, facilitando a busca e manipulação dos desafios apresentados ao jogador.

### Lista Circular
Implementada para gerenciar os obstáculos no jogo. A natureza circular da estrutura permite um fluxo contínuo de obstáculos, facilitando a criação de padrões cíclicos e a reutilização eficiente de elementos.

### Pilha
Usada para controlar os números de dano que aparecem quando o jogador é atingido. A estrutura LIFO (Last In, First Out) garante que os números mais recentes sejam processados primeiro, criando um efeito visual de sobreposição.

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
O jogo integra-se com a API Gemini do Google para gerar charadas dinâmicas durante o gameplay. Esta integração é implementada no arquivo `charada.c` e permite:

- Geração de charadas personalizadas com base na fase atual do jogo
- Criação de alternativas de resposta para cada charada
- Adaptação do nível de dificuldade das charadas conforme o progresso do jogador

A implementação utiliza a biblioteca libcurl para fazer requisições HTTP à API e a biblioteca json-c para processar as respostas JSON recebidas. As charadas geradas pela IA são armazenadas na lista duplamente encadeada para uso durante o jogo.

### IA para Comportamento de Inimigos
Além disso, na fase final do jogo, os obstáculos utilizam um algoritmo de perseguição ao jogador, ajustando sua trajetória com base na posição do coração. Esta implementação demonstra conceitos básicos de inteligência artificial em jogos.

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