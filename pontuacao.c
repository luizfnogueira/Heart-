#include "raylib.h"
#include "pontuacao.h"
#include "menu.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Definições de constantes
#define LARGURA_TELA 800
#define ALTURA_TELA 600
#define MAX_PONTUACOES_EXIBIDAS 10

// Implementação das funções de gerenciamento de pontuações

// Inicializa a lista de pontuações
ListaPontuacao* inicializarListaPontuacao(void) {
    ListaPontuacao* lista = (ListaPontuacao*)malloc(sizeof(ListaPontuacao));
    if (lista != NULL) {
        lista->cabeca = NULL;
        lista->maiorPontuacao = 0;
        strcpy(lista->nomeMaiorPontuacao, "");
    }
    return lista;
}

// Adiciona uma nova pontuação à lista
void adicionarPontuacao(ListaPontuacao* lista, const char* nome, int pontuacao) {
    if (lista == NULL) return;
    
    // Cria um novo nó
    NoPontuacao* novoNo = (NoPontuacao*)malloc(sizeof(NoPontuacao));
    if (novoNo == NULL) return;
    
    // Inicializa o novo nó
    strncpy(novoNo->nome, nome, 49);
    novoNo->nome[49] = '\0';
    novoNo->pontuacao = pontuacao;
    
    // Insere o novo nó no início da lista
    novoNo->proximo = lista->cabeca;
    lista->cabeca = novoNo;
    
    // Atualiza a maior pontuação se necessário
    if (pontuacao > lista->maiorPontuacao) {
        lista->maiorPontuacao = pontuacao;
        strncpy(lista->nomeMaiorPontuacao, nome, 49);
        lista->nomeMaiorPontuacao[49] = '\0';
    }
    
    // Ordena a lista usando Bubble Sort (requisito do projeto)
    ordenarPontuacoesBubbleSort(lista);
}

// Função auxiliar para ordenar as pontuações usando Bubble Sort
void ordenarPontuacoesBubbleSort(ListaPontuacao* lista) {
    if (lista == NULL || lista->cabeca == NULL || lista->cabeca->proximo == NULL) {
        return; // Lista vazia ou com apenas um elemento
    }
    
    int trocaRealizada;
    NoPontuacao* atual;
    NoPontuacao* anterior = NULL;
    
    do {
        trocaRealizada = 0;
        atual = lista->cabeca;
        
        while (atual->proximo != NULL) {
            // Se a pontuação atual for menor que a próxima, troca
            if (atual->pontuacao < atual->proximo->pontuacao) {
                // Troca os nós
                NoPontuacao* proximo = atual->proximo;
                atual->proximo = proximo->proximo;
                proximo->proximo = atual;
                
                // Atualiza os ponteiros
                if (anterior == NULL) {
                    lista->cabeca = proximo;
                } else {
                    anterior->proximo = proximo;
                }
                
                anterior = proximo;
                trocaRealizada = 1;
            } else {
                anterior = atual;
                atual = atual->proximo;
            }
        }
        
        // Reinicia o ponteiro anterior para a próxima iteração
        anterior = NULL;
        
    } while (trocaRealizada);
}

// Salva as pontuações em um arquivo
void salvarPontuacoes(ListaPontuacao* lista) {
    if (lista == NULL) return;
    
    FILE* arquivo = fopen("pontuacoes.txt", "w");
    if (arquivo == NULL) {
        TraceLog(LOG_ERROR, "Erro ao abrir arquivo de pontuações para escrita");
        return;
    }
    
    // Escreve a maior pontuação primeiro
    fprintf(arquivo, "%d\n%s\n", lista->maiorPontuacao, lista->nomeMaiorPontuacao);
    
    // Escreve todas as pontuações
    NoPontuacao* atual = lista->cabeca;
    while (atual != NULL) {
        fprintf(arquivo, "%s %d\n", atual->nome, atual->pontuacao);
        atual = atual->proximo;
    }
    
    fclose(arquivo);
}

// Carrega as pontuações de um arquivo
void carregarPontuacoes(ListaPontuacao* lista) {
    if (lista == NULL) return;
    
    FILE* arquivo = fopen("pontuacoes.txt", "r");
    if (arquivo == NULL) {
        TraceLog(LOG_WARNING, "Arquivo de pontuações não encontrado");
        return;
    }
    
    // Lê a maior pontuação primeiro
    fscanf(arquivo, "%d\n", &lista->maiorPontuacao);
    fgets(lista->nomeMaiorPontuacao, 50, arquivo);
    
    // Remove o caractere de nova linha, se presente
    size_t len = strlen(lista->nomeMaiorPontuacao);
    if (len > 0 && lista->nomeMaiorPontuacao[len-1] == '\n') {
        lista->nomeMaiorPontuacao[len-1] = '\0';
    }
    
    // Lê todas as pontuações
    char nome[50];
    int pontuacao;
    
    while (fscanf(arquivo, "%s %d\n", nome, &pontuacao) == 2) {
        adicionarPontuacao(lista, nome, pontuacao);
    }
    
    fclose(arquivo);
}

// Libera a memória alocada para a lista de pontuações
void liberarListaPontuacao(ListaPontuacao* lista) {
    if (lista == NULL) return;
    
    NoPontuacao* atual = lista->cabeca;
    NoPontuacao* proximo;
    
    while (atual != NULL) {
        proximo = atual->proximo;
        free(atual);
        atual = proximo;
    }
    
    free(lista);
}

// Funções para a tela de ranking
int atualizarTelaRanking(ListaPontuacao* lista) {
    if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_ESCAPE)) {
        return TELA_MENU;
    }
    return TELA_RANKING;
}

void desenharTelaRanking(ListaPontuacao* lista) {
    DrawText("💜 RANKING 💜", LARGURA_TELA/2 - MeasureText("💜 RANKING 💜", 50)/2, 30, 50, PURPLE);
    
    // Desenha a maior pontuação
    char textoMaiorPontuacao[100];
    sprintf(textoMaiorPontuacao, "Maior Pontuação: %d - %s", 
            lista->maiorPontuacao, 
            lista->nomeMaiorPontuacao);
    
    DrawText(textoMaiorPontuacao, 
             LARGURA_TELA/2 - MeasureText(textoMaiorPontuacao, 25)/2, 
             100, 
             25, 
             GOLD);
    
    // Cabeçalho da tabela
    DrawRectangle(150, 145, 500, 30, DARKPURPLE);
    DrawText("Posição", 160, 150, 20, WHITE);
    DrawText("Nome", 300, 150, 20, WHITE);
    DrawText("Pontuação", 550, 150, 20, WHITE);
    
    // Desenha uma linha separadora
    DrawLine(150, 175, 650, 175, LIGHTGRAY);
    
    // Desenha as pontuações
    NoPontuacao* atual = lista->cabeca;
    int posicao = 1;
    
    while (atual != NULL && posicao <= MAX_PONTUACOES_EXIBIDAS) {
        // Fundo alternado para melhor visualização
        if (posicao % 2 == 0) {
            DrawRectangle(150, 175 + posicao * 30, 500, 25, (Color){45, 45, 65, 128});
        }
        
        // Posição
        char textoPos[10];
        sprintf(textoPos, "%d", posicao);
        Color corPosicao = WHITE;
        if (posicao <= 3) {
            // Cores especiais para os 3 primeiros lugares
            corPosicao = (posicao == 1) ? GOLD : (posicao == 2) ? LIGHTGRAY : ORANGE;
        }
        DrawText(textoPos, 170, 180 + posicao * 30, 20, corPosicao);
        
        // Nome
        DrawText(atual->nome, 300, 180 + posicao * 30, 20, WHITE);
        
        // Pontuação
        char textoPont[20];
        sprintf(textoPont, "%d", atual->pontuacao);
        DrawText(textoPont, 550, 180 + posicao * 30, 20, WHITE);
        
        atual = atual->proximo;
        posicao++;
    }
    
    // Se não houver pontuações
    if (lista->cabeca == NULL) {
        DrawRectangle(150, 220, 500, 100, (Color){45, 45, 65, 200});
        DrawText("Nenhuma pontuação registrada ainda", 
                 LARGURA_TELA/2 - MeasureText("Nenhuma pontuação registrada ainda", 25)/2, 
                 250, 
                 25, 
                 LIGHTGRAY);
        DrawText("Jogue para registrar seu nome aqui!", 
                 LARGURA_TELA/2 - MeasureText("Jogue para registrar seu nome aqui!", 20)/2, 
                 290, 
                 20, 
                 PURPLE);
    }
    
    DrawText("Pressione ENTER ou ESC para voltar ao menu", 
             LARGURA_TELA/2 - MeasureText("Pressione ENTER ou ESC para voltar ao menu", 20)/2, 
             ALTURA_TELA - 40, 
             20, 
             LIGHTGRAY);
}
