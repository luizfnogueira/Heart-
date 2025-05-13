#ifndef BOSS_H
#define BOSS_H

#include "jogo.h"   // Definições de Boss, MAX_BOSSES, AREA_JOGO_*

// Inicializa todos os bosses carregando a sprite externa
void inicializarBosses(void);

// Desenha todos os bosses ativos
void desenharBosses(void);

// Descarrega a textura única dos bosses
void UnloadBossTexture(void);

// Variável global de bosses (definida em jogo.c)
extern Boss bosses[MAX_BOSSES];

#endif // BOSS_H
