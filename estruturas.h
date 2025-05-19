#ifndef ESTRUTURAS_H
#define ESTRUTURAS_H

#include "jogo.h"

// =============================================================================
// Implementação de Lista Circular para Obstáculos
// =============================================================================

// Nó da lista circular de obstáculos
typedef struct NodoObstaculo {
    Obstaculo obstaculo;
    struct NodoObstaculo* proximo;
} NodoObstaculo;

// Lista circular de obstáculos
typedef struct {
    NodoObstaculo* ultimo;  // Aponta para o último nó, cujo próximo é o primeiro
    int tamanho;
} FilaObstaculos;

// Funções de manipulação da lista circular
void inicializarFilaObstaculos(FilaObstaculos* fila);
bool filaObstaculosVazia(FilaObstaculos* fila);
bool enfileirarObstaculo(FilaObstaculos* fila, Obstaculo obs);
bool desenfileirarObstaculo(FilaObstaculos* fila, Obstaculo* obs);
void percorrerObstaculos(FilaObstaculos* fila, void (*callback)(Obstaculo*));

// =============================================================================
// Implementação de Pilha para Números de Dano
// =============================================================================

typedef struct {
    NumeroDano itens[MAX_NUMEROS_DANO];
    int topo;
} PilhaNumerosDano;

// Funções de manipulação da pilha
void inicializarPilhaNumerosDano(PilhaNumerosDano* pilha);
bool pilhaNumerosDanoCheia(PilhaNumerosDano* pilha);
bool pilhaNumerosDanoVazia(PilhaNumerosDano* pilha);
bool empilharNumeroDano(PilhaNumerosDano* pilha, NumeroDano num);
bool desempilharNumeroDano(PilhaNumerosDano* pilha, NumeroDano* num);

// =============================================================================
// Implementação de Lista Encadeada para Charadas
// =============================================================================

// Nó da lista duplamente encadeada de charadas
typedef struct NodoCharada {
    Charada charada;
    struct NodoCharada* proximo;
    struct NodoCharada* anterior; // Ponteiro para o nó anterior
} NodoCharada;

// Lista duplamente encadeada de charadas
typedef struct {
    NodoCharada* inicio;
    NodoCharada* fim;
    int tamanho;
} ListaCharadas;

// Funções de manipulação da lista
void inicializarListaCharadas(ListaCharadas* lista);
bool listaCharadasVazia(ListaCharadas* lista);
bool inserirCharada(ListaCharadas* lista, Charada charada);
bool removerCharada(ListaCharadas* lista, int indice, Charada* charada);
void liberarListaCharadas(ListaCharadas* lista);
Charada* buscarCharada(ListaCharadas* lista, int indice);

// =============================================================================
// Implementação de Árvore Binária para Progressão de Bosses
// =============================================================================

// Nó da árvore de bosses
typedef struct NodoArvore {
    Boss boss;
    struct NodoArvore* esquerda;
    struct NodoArvore* direita;
} NodoArvore;

// Árvore de bosses
typedef struct {
    NodoArvore* raiz;
} ArvoreBosses;

// Funções de manipulação da árvore
void inicializarArvoreBosses(ArvoreBosses* arvore);
NodoArvore* criarNodoBoss(Boss boss);
void inserirBoss(ArvoreBosses* arvore, Boss boss);
NodoArvore* buscarBoss(ArvoreBosses* arvore, int fase);
void liberarArvoreBosses(ArvoreBosses* arvore);
void percorrerEmOrdem(NodoArvore* nodo, void (*callback)(Boss*));

#endif // ESTRUTURAS_H
