#include "estruturas.h"
#include <stdlib.h>
#include <string.h>

// =============================================================================
// Implementação da Lista Circular de Obstáculos
// =============================================================================

void inicializarFilaObstaculos(FilaObstaculos* fila) {
    fila->ultimo = NULL;
    fila->tamanho = 0;
}

bool filaObstaculosVazia(FilaObstaculos* fila) {
    return fila->ultimo == NULL;
}

bool enfileirarObstaculo(FilaObstaculos* fila, Obstaculo obs) {
    NodoObstaculo* novoNodo = (NodoObstaculo*)malloc(sizeof(NodoObstaculo));
    if (novoNodo == NULL) {
        return false; // Falha na alocação de memória
    }
    
    novoNodo->obstaculo = obs;
    
    if (filaObstaculosVazia(fila)) {
        // Lista vazia: o novo nó aponta para si mesmo
        novoNodo->proximo = novoNodo;
        fila->ultimo = novoNodo;
    } else {
        // Inserir após o último (que se torna o novo último)
        novoNodo->proximo = fila->ultimo->proximo; // Aponta para o primeiro
        fila->ultimo->proximo = novoNodo; // O antigo último aponta para o novo
        fila->ultimo = novoNodo; // Atualiza o último
    }
    
    fila->tamanho++;
    return true;
}

bool desenfileirarObstaculo(FilaObstaculos* fila, Obstaculo* obs) {
    if (filaObstaculosVazia(fila)) {
        return false; // Lista vazia
    }
    
    NodoObstaculo* primeiro = fila->ultimo->proximo;
    *obs = primeiro->obstaculo;
    
    if (primeiro == fila->ultimo) {
        // Último elemento da lista
        free(primeiro);
        fila->ultimo = NULL;
    } else {
        // Remove o primeiro elemento
        fila->ultimo->proximo = primeiro->proximo;
        free(primeiro);
    }
    
    fila->tamanho--;
    return true;
}

void percorrerObstaculos(FilaObstaculos* fila, void (*callback)(Obstaculo*)) {
    if (filaObstaculosVazia(fila)) {
        return;
    }
    
    NodoObstaculo* atual = fila->ultimo->proximo; // Primeiro elemento
    do {
        callback(&atual->obstaculo);
        atual = atual->proximo;
    } while (atual != fila->ultimo->proximo); // Continua até voltar ao primeiro
}

// =============================================================================
// Implementação da Pilha de Números de Dano
// =============================================================================

void inicializarPilhaNumerosDano(PilhaNumerosDano* pilha) {
    pilha->topo = -1; // Pilha vazia
}

bool pilhaNumerosDanoCheia(PilhaNumerosDano* pilha) {
    return pilha->topo >= MAX_NUMEROS_DANO - 1;
}

bool pilhaNumerosDanoVazia(PilhaNumerosDano* pilha) {
    return pilha->topo == -1;
}

bool empilharNumeroDano(PilhaNumerosDano* pilha, NumeroDano num) {
    if (pilhaNumerosDanoCheia(pilha)) {
        return false; // Pilha cheia
    }
    
    pilha->topo++;
    pilha->itens[pilha->topo] = num;
    return true;
}

bool desempilharNumeroDano(PilhaNumerosDano* pilha, NumeroDano* num) {
    if (pilhaNumerosDanoVazia(pilha)) {
        return false; // Pilha vazia
    }
    
    *num = pilha->itens[pilha->topo];
    pilha->topo--;
    return true;
}

// =============================================================================
// Implementação da Lista Duplamente Encadeada de Mensagens de Conforto
// =============================================================================

void inicializarListaMensagens(ListaMensagens* lista) {
    lista->inicio = NULL;
    lista->fim = NULL;
    lista->tamanho = 0;
}

bool listaMensagensVazia(ListaMensagens* lista) {
    return lista->tamanho == 0;
}

bool inserirMensagem(ListaMensagens* lista, MensagemConforto mensagem) {
    NodoMensagem* novoNodo = (NodoMensagem*)malloc(sizeof(NodoMensagem));
    if (novoNodo == NULL) {
        return false; // Erro de alocação
    }

    novoNodo->mensagem = mensagem;
    novoNodo->proximo = NULL;
    novoNodo->anterior = NULL;

    if (lista->inicio == NULL) {
        lista->inicio = novoNodo;
        lista->fim = novoNodo;
    } else {
        novoNodo->anterior = lista->fim;
        lista->fim->proximo = novoNodo;
        lista->fim = novoNodo;
    }

    lista->tamanho++;
    return true;
}

bool removerMensagem(ListaMensagens* lista, int indice, MensagemConforto* mensagem) {
    if (listaMensagensVazia(lista) || indice < 0 || indice >= lista->tamanho) {
        return false;
    }

    NodoMensagem* atual = lista->inicio;
    for (int i = 0; i < indice; i++) {
        atual = atual->proximo;
    }

    if (atual->anterior != NULL) {
        atual->anterior->proximo = atual->proximo;
    } else {
        lista->inicio = atual->proximo;
    }

    if (atual->proximo != NULL) {
        atual->proximo->anterior = atual->anterior;
    } else {
        lista->fim = atual->anterior;
    }

    *mensagem = atual->mensagem;
    free(atual);
    lista->tamanho--;
    return true;
}

void liberarListaMensagens(ListaMensagens* lista) {
    NodoMensagem* atual = lista->inicio;
    while (atual != NULL) {
        NodoMensagem* proximo = atual->proximo;
        free(atual);
        atual = proximo;
    }

    lista->inicio = NULL;
    lista->fim = NULL;
    lista->tamanho = 0;
}

MensagemConforto* buscarMensagem(ListaMensagens* lista, int indice) {
    if (listaMensagensVazia(lista) || indice < 0 || indice >= lista->tamanho) {
        return NULL;
    }

    NodoMensagem* atual = lista->inicio;
    for (int i = 0; i < indice; i++) {
        atual = atual->proximo;
    }

    return &atual->mensagem;
}

// =============================================================================
// Implementação da Árvore de Bosses
// =============================================================================

void inicializarArvoreBosses(ArvoreBosses* arvore) {
    arvore->raiz = NULL;
}

NodoArvore* criarNodoBoss(Boss boss) {
    NodoArvore* novoNodo = (NodoArvore*)malloc(sizeof(NodoArvore));
    if (novoNodo != NULL) {
        novoNodo->boss = boss;
        novoNodo->esquerda = NULL;
        novoNodo->direita = NULL;
    }
    return novoNodo;
}

// Função auxiliar para inserção na árvore
static NodoArvore* inserirNodo(NodoArvore* raiz, Boss boss) {
    if (raiz == NULL) {
        return criarNodoBoss(boss);
    }
    
    // Ordenando pela fase do boss
    if (boss.fase < raiz->boss.fase) {
        raiz->esquerda = inserirNodo(raiz->esquerda, boss);
    } else if (boss.fase > raiz->boss.fase) {
        raiz->direita = inserirNodo(raiz->direita, boss);
    } else {
        // Fase igual, atualiza o nodo existente
        raiz->boss = boss;
    }
    
    return raiz;
}

void inserirBoss(ArvoreBosses* arvore, Boss boss) {
    arvore->raiz = inserirNodo(arvore->raiz, boss);
}

// Função auxiliar para busca na árvore
static NodoArvore* buscarNodo(NodoArvore* raiz, int fase) {
    if (raiz == NULL || raiz->boss.fase == fase) {
        return raiz;
    }
    
    if (fase < raiz->boss.fase) {
        return buscarNodo(raiz->esquerda, fase);
    } else {
        return buscarNodo(raiz->direita, fase);
    }
}

NodoArvore* buscarBoss(ArvoreBosses* arvore, int fase) {
    return buscarNodo(arvore->raiz, fase);
}

// Função auxiliar para percorrer a árvore em ordem
void percorrerEmOrdemAux(NodoArvore* nodo, void (*callback)(Boss*)) {
    if (nodo != NULL) {
        percorrerEmOrdemAux(nodo->esquerda, callback);
        callback(&nodo->boss);
        percorrerEmOrdemAux(nodo->direita, callback);
    }
}

void percorrerEmOrdem(NodoArvore* nodo, void (*callback)(Boss*)) {
    percorrerEmOrdemAux(nodo, callback);
}

// Função auxiliar para liberar a árvore
static void liberarNodos(NodoArvore* nodo) {
    if (nodo != NULL) {
        liberarNodos(nodo->esquerda);
        liberarNodos(nodo->direita);
        free(nodo);
    }
}

void liberarArvoreBosses(ArvoreBosses* arvore) {
    liberarNodos(arvore->raiz);
    arvore->raiz = NULL;
}
