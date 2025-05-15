// menu.h
#ifndef MENU_H
#define MENU_H

#include "pontuacao.h"

// Definição dos tipos de tela
typedef enum {
    TELA_MENU = 0,
    TELA_JOGO,
    TELA_CREDITOS,
    TELA_HISTORIA,
    TELA_RANKING,
    TELA_CONTROLES,
    TELA_GAMEOVER,
    TELA_SAIR
} TipoTela;

// Funções do menu
int atualizarMenu(void);
void desenharMenu(void);

// Funções das telas adicionais
int atualizarTelaCreditos(void);
void desenharTelaCreditos(void);
int atualizarTelaHistoria(void);
void desenharTelaHistoria(void);
int atualizarTelaGameOver(ListaPontuacao* listaPontuacoes);
void desenharTelaGameOver(void);
void definirPontuacaoFinal(int pontuacao);
int atualizarTelaControles(void);
void desenharTelaControles(void);

#endif // MENU_H
