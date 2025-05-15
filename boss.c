// boss.c
#include "boss.h"
#include <raylib.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// Nomes e descrições locais
static const char* nomesBosses[] = {
    "Memória Dolorosa",
    "Trauma Profundo",
    "Emoção Caótica"
};
static const char* descricoesBosses[] = {
    "Uma lembrança que se recusa a ser esquecida, atacando com padrões repetitivos e implacáveis.",
    "Um trauma que se manifesta como uma entidade sombria, perseguindo o coração com ataques calculados.",
    "A personificação do caos emocional, com ataques imprevisíveis e devastadores."
};

// Textura única do boss, carregada apenas uma vez
static Texture2D bossTexture = { 0 };

void inicializarBosses(void) {
    // Carrega sprite externa na primeira chamada
    if (!bossTexture.id) {
        bossTexture = LoadTexture("recursos/Sprites/FlameDemon.png");
        if (bossTexture.id == 0) TraceLog(LOG_ERROR, "Falha ao carregar FlameDemon.png");
    }

    for (int i = 0; i < MAX_BOSSES; i++) {
        // Propriedades básicas
        bosses[i].posicao           = (Vector2){ AREA_JOGO_X + AREA_JOGO_LARGURA/2, AREA_JOGO_Y + 100 };
        bosses[i].vida              = 500.0f * (i + 1);
        bosses[i].vidaMaxima        = bosses[i].vida;
        bosses[i].fase              = i + 1;
        bosses[i].ativo             = false;
        bosses[i].tempo             = 0.0f;
        bosses[i].tempoAtaque       = 0.0f;
        bosses[i].tempoEntreAtaques = 2.0f - (i * 0.3f);
        bosses[i].padraoAtual       = GetRandomValue(0, MAX_PADROES_ATAQUE - 1);

        // Usa a sprite externa
        bosses[i].textura    = bossTexture;
        bosses[i].numFrames  = 1;
        bosses[i].frameAtual = (Rectangle){
            0, 0,
            (float)bossTexture.width,
            (float)bossTexture.height
        };
        bosses[i].tempoFrame = 0.0f;

        // Nome e descrição
        strcpy(bosses[i].nome,      nomesBosses[i]);
        strcpy(bosses[i].descricao, descricoesBosses[i]);

        // Inicializa projéteis
        for (int j = 0; j < MAX_PROJETEIS; j++) {
            bosses[i].projeteis[j].ativo     = false;
            bosses[i].projeteis[j].usaSprite = false;
            bosses[i].projeteis[j].rotacao   = 0.0f;
            bosses[i].projeteis[j].escala    = 1.0f;
        }

        // Controle por IA
        // bosses[i].inteligente = true;
        // strcpy(bosses[i].ultimaResposta, "Aguardando primeira decisão da IA...");
    }

    printf("Bosses inicializados com sucesso!\n");
}

void desenharBosses(void) {
    for (int i = 0; i < MAX_BOSSES; i++) {
        if (!bosses[i].ativo) continue;
        DrawTextureRec(
            bosses[i].textura,
            bosses[i].frameAtual,
            bosses[i].posicao,
            WHITE
        );
    }
}

void UnloadBossTexture(void) {
    if (bossTexture.id) {
        UnloadTexture(bossTexture);
        bossTexture.id = 0;
    }
}
