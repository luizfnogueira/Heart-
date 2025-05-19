#ifndef GEMINI_H
#define GEMINI_H

#include "raylib.h"

// Estrutura para armazenar mensagens de conforto
typedef struct {
    char mensagem[512];
    bool ativa;
} MensagemConforto;

// Funções principais
void inicializarMensagensConforto(void);
void atualizarMensagemConforto(void);
void desenharMensagemConforto(void);
void liberarRecursosMensagemConforto(void);

// Variável externa
extern MensagemConforto mensagemAtual;

#endif // CHARADA_H
