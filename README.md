# 💜 HEART! 💜

Uma reimplementação do jogo Heart utilizando a biblioteca Raylib.

## 📝 Descrição

Este projeto é uma reimplementação do jogo Heart original, desenvolvido para a disciplina de Análise e Estrutura de Dados. O jogo consiste em controlar um coração que deve desviar de obstáculos para sobreviver.

### Público Alvo
Estudantes e entusiastas de jogos que apreciam desafios de desviar de obstáculos com mecânicas simples mas envolventes.

### Narrativa
Em um mundo onde a luz se apagou, você é a última centelha de um coração que já foi cheio de vida. Preso em uma dimensão entre o vazio e a esperança, você enfrenta os ecos sombrios dos medos e arrependimentos de uma alma perdida. Cada obstáculo superado é uma luta para recuperar fragmentos de sentimentos e memórias esquecidas.

## 🎮 Como Jogar

- **Movimentação**: Utilize as teclas W, A, S, D para mover o coração.
- **Obstáculos**:
  - **Obstáculos Brancos**: Desvie a todo custo! Colidir com eles causa dano ao coração.
  - **Obstáculos Roxos**: Podem ser atravessados sem dano, mas apenas se você permanecer imóvel ao passar por eles.
  - **Obstáculos Amarelos**: Colete-os para recuperar vida.

## 🎯 Objetivo

O objetivo do jogo é sobreviver o máximo possível, acumulando pontos e avançando pelas três fases do jogo até derrotar o chefe final e restaurar completamente o coração.

## 🧠 Estruturas de Dados Utilizadas

### Lista Encadeada
Utilizada para armazenar e gerenciar as pontuações dos jogadores no ranking.

**Justificativa**: A lista encadeada foi escolhida por sua flexibilidade na inserção e remoção de elementos, permitindo um gerenciamento eficiente das pontuações sem necessidade de realocação de memória, como seria o caso em um array.

## 🔄 Algoritmo de Ordenação

### Bubble Sort
Utilizado para ordenar as pontuações no ranking.

**Justificativa**: O Bubble Sort foi escolhido por sua simplicidade de implementação e eficiência para pequenas quantidades de dados, como é o caso do ranking de pontuações. Embora não seja o algoritmo mais eficiente para grandes conjuntos de dados, para a escala deste jogo, ele é adequado e facilmente compreensível.

## 🤖 Funcionalidades de IA

Na fase final do jogo, os obstáculos utilizam um algoritmo simples de perseguição ao jogador, ajustando sua trajetória com base na posição do coração. Esta implementação demonstra conceitos básicos de inteligência artificial em jogos.

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

1. Certifique-se de ter a biblioteca Raylib instalada
2. Compile o projeto usando:
   ```
   gcc -o heart main.c menu.c jogo.c pontuacao.c -lraylib -lGL -lm -lpthread -ldl -lrt -lX11
   ```
3. Execute o jogo:
   ```
   ./heart
   ```

---

*Este projeto foi desenvolvido como parte da disciplina de Análise e Estrutura de Dados.*