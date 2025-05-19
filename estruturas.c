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
// Implementação da Lista Duplamente Encadeada de Charadas
// =============================================================================

void inicializarListaCharadas(ListaCharadas* lista) {
    lista->inicio = NULL;
    lista->fim = NULL;
    lista->tamanho = 0;
}

bool listaCharadasVazia(ListaCharadas* lista) {
    return lista->tamanho == 0;
}

bool inserirCharada(ListaCharadas* lista, Charada charada) {
    NodoCharada* novoNodo = (NodoCharada*)malloc(sizeof(NodoCharada));
    if (novoNodo == NULL) {
        return false; // Erro de alocação
    }
    
    novoNodo->charada = charada;
    novoNodo->proximo = NULL;
    novoNodo->anterior = NULL; // Inicializa o ponteiro anterior
    
    if (lista->inicio == NULL) {
        // Lista vazia
        lista->inicio = novoNodo;
        lista->fim = novoNodo;
    } else {
        // Adicionar no fim
        novoNodo->anterior = lista->fim; // O novo nó aponta para o antigo fim
        lista->fim->proximo = novoNodo; // O antigo fim aponta para o novo nó
        lista->fim = novoNodo; // Atualiza o fim
    }
    
    lista->tamanho++;
    return true;
}

bool removerCharada(ListaCharadas* lista, int indice, Charada* charada) {
    if (listaCharadasVazia(lista) || indice < 0 || indice >= lista->tamanho) {
        return false;
    }
    
    NodoCharada* atual = lista->inicio;
    
    // Encontrar o nodo a ser removido
    for (int i = 0; i < indice; i++) {
        atual = atual->proximo;
    }
    
    // Copiar dados para retorno
    if (charada != NULL) {
        *charada = atual->charada;
    }
    
    // Remover o nodo
    if (atual->anterior == NULL) {
        // Remover o primeiro nodo
        lista->inicio = atual->proximo;
        if (lista->inicio != NULL) {
            lista->inicio->anterior = NULL;
        }
    } else {
        // Nodo do meio ou do fim
        atual->anterior->proximo = atual->proximo;
    }
    
    // Se não é o último, atualizar o anterior do próximo
    if (atual->proximo != NULL) {
        atual->proximo->anterior = atual->anterior;
    } else {
        // É o último, atualizar o fim
        lista->fim = atual->anterior;
    }
    
    free(atual);
    lista->tamanho--;
    
    return true;
}

void liberarListaCharadas(ListaCharadas* lista) {
    NodoCharada* atual = lista->inicio;
    while (atual != NULL) {
        NodoCharada* temp = atual;
        atual = atual->proximo;
        free(temp);
    }
    
    lista->inicio = NULL;
    lista->fim = NULL;
    lista->tamanho = 0;
}

Charada* buscarCharada(ListaCharadas* lista, int indice) {
    if (listaCharadasVazia(lista) || indice < 0 || indice >= lista->tamanho) {
        return NULL;
    }
    
    NodoCharada* atual = lista->inicio;
    for (int i = 0; i < indice; i++) {
        atual = atual->proximo;
    }
    
    return &(atual->charada);
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
