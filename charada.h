#ifndef CHARADA_H
#define CHARADA_H

#include "raylib.h"

// Estrutura para armazenar dados da charada
typedef struct {
    char pergunta[512];
    char resposta[128];
    char tentativaAtual[128];
    char alternativas[3][64]; // Adicionado para armazenar as alternativas de resposta
    int tentativasRestantes;
    bool ativa;
    bool respondida;
    bool acertou;
    int faseAlvo;
} Charada;

// Funções principais
void inicializarSistemaCharadas(void);
bool atualizarTelaCharada(void);
void desenharTelaCharada(void);
bool verificarResposta(void);
void gerarNovaCharada(int fase);
void liberarRecursosCharada(void);

// Variáveis externas
extern Charada charadaAtual;
extern bool modoCharada;

#endif // CHARADA_H
