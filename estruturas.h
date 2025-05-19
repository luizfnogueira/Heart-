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
// Implementação de Lista Encadeada para Mensagens de Conforto
// =============================================================================

// Nó da lista duplamente encadeada de mensagens
typedef struct NodoMensagem {
    MensagemConforto mensagem;
    struct NodoMensagem* proximo;
    struct NodoMensagem* anterior; // Ponteiro para o nó anterior
} NodoMensagem;

// Lista duplamente encadeada de mensagens
typedef struct {
    NodoMensagem*  inicio;
    NodoMensagem* fim;
    int tamanho;
} ListaMensagens;

// Funções de manipulação da lista
void inicializarListaMensagens(ListaMensagens* lista);
bool listaMensagensVazia(ListaMensagens* lista);
bool inserirMensagem(ListaMensagens* lista, MensagemConforto mensagem);
bool removerMensagem(ListaMensagens* lista, int indice, MensagemConforto* mensagem);
void liberarListaMensagens(ListaMensagens* lista);
MensagemConforto* buscarMensagem(ListaMensagens* lista, int indice);

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
