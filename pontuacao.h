// pontuacao.h
#ifndef PONTUACAO_H
#define PONTUACAO_H

// Estrutura para armazenar um nó da lista de pontuações
typedef struct NoPontuacao {
    char nome[50];
    int pontuacao;
    struct NoPontuacao* proximo;
} NoPontuacao;

// Estrutura para gerenciar a lista de pontuações
typedef struct ListaPontuacao {
    NoPontuacao* cabeca;
    int maiorPontuacao;
    char nomeMaiorPontuacao[50];
} ListaPontuacao;

// Funções para gerenciamento de pontuações
ListaPontuacao* inicializarListaPontuacao(void);
void adicionarPontuacao(ListaPontuacao* lista, const char* nome, int pontuacao);
void ordenarPontuacoesBubbleSort(ListaPontuacao* lista);
void salvarPontuacoes(ListaPontuacao* lista);
void carregarPontuacoes(ListaPontuacao* lista);
void liberarListaPontuacao(ListaPontuacao* lista);
int atualizarTelaRanking(ListaPontuacao* lista);
void desenharTelaRanking(ListaPontuacao* lista);

#endif // PONTUACAO_H
