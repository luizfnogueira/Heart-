#include "boss.h"
#include "estruturas.h"  // Incluir para ArvoreBosses
#include <raylib.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define TOTAL_FASES 4

extern Boss bosses[MAX_BOSSES];

// Árvore binária de bosses
ArvoreBosses arvoreBosses;

// Array de texturas para os bosses
static Texture2D bossTextures[4] = {0};

static const char* nomesBosses[] = {
    "Memória Dolorosa",
    "Trauma Profundo",
    "Emoção Caótica",
    "Sombra Final"
};
static const char* descricoesBosses[] = {
    "Uma lembrança que se recusa a ser esquecida, atacando com padrões repetitivos e implacáveis.",
    "Um trauma que se manifesta como uma entidade sombria, perseguindo o coração com ataques calculados.",
    "A personificação do caos emocional, com ataques imprevisíveis e devastadores.",
    "A última sombra, que reflete todos os medos vividos e não vividos."
};

void inicializarBosses(void) {
    // Inicializar a árvore de bosses
    inicializarArvoreBosses(&arvoreBosses);
    
    // Carrega as sprites dos bosses (uma por fase)
    bossTextures[0] = LoadTexture("recursos/Sprites/sans1_sheet.png");
    bossTextures[1] = LoadTexture("recursos/Sprites/sans2_sheet.png");
    bossTextures[2] = LoadTexture("recursos/Sprites/sans3_sheet.png");
    bossTextures[3] = LoadTexture("recursos/Sprites/sans4_sheet.png");

    for (int i = 0; i < MAX_BOSSES; i++) {
        int faseIndex = (i % TOTAL_FASES);
        bosses[i].textura = bossTextures[faseIndex];

        // Centraliza o boss no meio do quadrado do boss e ajusta escala para encaixar perfeitamente
        float frameWidth = (float)bosses[i].textura.width / 4;
        float frameHeight = (float)bosses[i].textura.height;
        float escala = ALTURA_QUADRADO_BOSS / frameHeight; // Escala para encaixar altura no quadrado
        if (frameWidth * escala > ALTURA_QUADRADO_BOSS) {
            escala = ALTURA_QUADRADO_BOSS / frameWidth; // Se largura for maior, ajusta pela largura
        }
        bosses[i].posicao.x = AREA_JOGO_X + (AREA_JOGO_LARGURA - frameWidth * escala) / 2;
        bosses[i].posicao.y = AREA_JOGO_Y - ALTURA_QUADRADO_BOSS + (ALTURA_QUADRADO_BOSS - frameHeight * escala) / 2; // Centraliza no quadrado superior

        bosses[i].vida              = 500.0f * (i + 1);
        bosses[i].vidaMaxima        = bosses[i].vida;
        bosses[i].fase              = i + 1;
        bosses[i].ativo             = false;
        bosses[i].tempo             = 0.0f;
        bosses[i].tempoAtaque       = 0.0f;
        bosses[i].tempoEntreAtaques = 2.0f - (i * 0.3f);
        bosses[i].padraoAtual       = GetRandomValue(0, MAX_PADROES_ATAQUE - 1);
        bosses[i].numFrames         = 4;

        bosses[i].frameAtual = (Rectangle){
            0, 0,
            frameWidth,
            frameHeight
        };
        bosses[i].tempoFrame = 0.0f;

        strcpy(bosses[i].nome,      nomesBosses[faseIndex]);
        strcpy(bosses[i].descricao, descricoesBosses[faseIndex]);        for (int j = 0; j < MAX_PROJETEIS; j++) {
            bosses[i].projeteis[j].ativo     = false;
            bosses[i].projeteis[j].usaSprite = false;
            bosses[i].projeteis[j].rotacao   = 0.0f;
            bosses[i].projeteis[j].escala    = 1.0f;
        }
        
        // Adicionar o boss à árvore
        inserirBoss(&arvoreBosses, bosses[i]);
    }

    printf("Bosses inicializados com sucesso!\n");
}

void desenharBosses(void) {
    for (int i = 0; i < MAX_BOSSES; i++) {
        if (!bosses[i].ativo) continue;

        // Atualiza frame de animação
        bosses[i].tempoFrame += GetFrameTime();
        if (bosses[i].tempoFrame >= 0.35f) {
            bosses[i].tempoFrame = 0.0f;
            float frameWidth = bosses[i].frameAtual.width;
            bosses[i].frameAtual.x += frameWidth;

            if (bosses[i].frameAtual.x >= bosses[i].textura.width) {
                bosses[i].frameAtual.x = 0;
            }
        }

        // Centraliza o boss no meio do quadrado do boss e ajusta escala
        float frameWidth = bosses[i].frameAtual.width;
        float frameHeight = bosses[i].frameAtual.height;
        float escala = ALTURA_QUADRADO_BOSS / frameHeight;
        if (frameWidth * escala > ALTURA_QUADRADO_BOSS) {
            escala = ALTURA_QUADRADO_BOSS / frameWidth;
        }
        float bossX = AREA_JOGO_X + (AREA_JOGO_LARGURA - frameWidth * escala) / 2;
        float bossY = AREA_JOGO_Y - ALTURA_QUADRADO_BOSS + (ALTURA_QUADRADO_BOSS - frameHeight * escala) / 2;

        DrawTexturePro(
            bosses[i].textura,
            bosses[i].frameAtual,
            (Rectangle){
                bossX,
                bossY,
                frameWidth * escala,
                frameHeight * escala
            },
            (Vector2){ 0, 0 },
            0.0f,
            WHITE
        );
    }
}

void UnloadBossTexture(void) {
    for (int i = 0; i < TOTAL_FASES; i++) {
        if (bossTextures[i].id) {
            UnloadTexture(bossTextures[i]);
            bossTextures[i].id = 0;
        }
    }
}
