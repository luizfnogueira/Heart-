/*******************************************************************************************
*
*   Heart - Um jogo de desviar de obstáculos
*
*   Este jogo é uma reimplementação do jogo Heart usando a biblioteca Raylib
*   
*   Controles:
*   - W, A, S, D - Movimentar o coração
*   - ESC - Sair do jogo
*
********************************************************************************************/

#include "raylib.h"
#include "menu.h"
#include "jogo.h"
#include "pontuacao.h"
#include "boss.h"           
#include "obstaculo_sprites.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Definições de constantes
#define LARGURA_TELA    800
#define ALTURA_TELA     600
#define TITULO_JOGO     "Heart!"

// Definição global da música para ser acessível na finalização
Music backgroundMusic;

int main(void) {
    // Inicialização do Raylib
    InitWindow(LARGURA_TELA, ALTURA_TELA, TITULO_JOGO);
    SetExitKey(KEY_NULL);  // Desativa ESC como atalho para fechar
    SetTargetFPS(60);
    
    // Inicialização do áudio
    InitAudioDevice();
    
    // Carregamento de recursos
    backgroundMusic = LoadMusicStream("recursos/Condemned Tower - Castlevania Dawn of Sorrow OST.mp3");
    if (backgroundMusic.frameCount == 0) {
        TraceLog(LOG_WARNING, "Falha ao carregar a música de fundo.");
    } else {
        PlayMusicStream(backgroundMusic);
        SetMusicVolume(backgroundMusic, 0.5f);
    }
    
    // Inicialização de variáveis do jogo
    int telaAtual = TELA_MENU;
    ListaPontuacao* listaPontuacoes = inicializarListaPontuacao();
    carregarPontuacoes(listaPontuacoes);
    
    // Inicializa bosses
    inicializarBosses();
    
    while (!WindowShouldClose()) {
        // Atualização de áudio
        if (IsMusicStreamPlaying(backgroundMusic)) UpdateMusicStream(backgroundMusic);
        
        // Lógica de tela
        switch (telaAtual) {
            case TELA_MENU:
                telaAtual = atualizarMenu();
                if (telaAtual == TELA_JOGO) {
                    PlayMusicStream(backgroundMusic);
                    inicializarJogo();
                }
                break;
            case TELA_JOGO:
                if (!atualizarJogo()) {
                    definirPontuacaoFinal((int)pontuacao);
                    telaAtual = TELA_GAMEOVER;
                }
                break;
            case TELA_CREDITOS:
                telaAtual = atualizarTelaCreditos();
                break;
            case TELA_HISTORIA:
                telaAtual = atualizarTelaHistoria();
                break;
            case TELA_RANKING:
                telaAtual = atualizarTelaRanking(listaPontuacoes);
                break;
            case TELA_CONTROLES:
                telaAtual = atualizarTelaControles();
                break;
            case TELA_GAMEOVER:
                StopMusicStream(backgroundMusic);
                telaAtual = atualizarTelaGameOver(listaPontuacoes);
                if (telaAtual == TELA_JOGO) {
                    PlayMusicStream(backgroundMusic);
                    inicializarJogo();
                }
                break;
            case TELA_SAIR:
                CloseWindow();
                break;
        }
        
        // Desenho
        BeginDrawing();
        ClearBackground(BLACK);
        switch (telaAtual) {
            case TELA_MENU:        desenharMenu(); break;
            case TELA_JOGO:        desenharJogo(); break;
            case TELA_CREDITOS:    desenharTelaCreditos(); break;
            case TELA_HISTORIA:    desenharTelaHistoria(); break;
            case TELA_RANKING:     desenharTelaRanking(listaPontuacoes); break;
            case TELA_CONTROLES:   desenharTelaControles(); break;
            case TELA_GAMEOVER:    desenharTelaGameOver(); break;
        }
        EndDrawing();
    }
    
    // Descarregamento de recursos
    UnloadMusicStream(backgroundMusic);
    liberarListaPontuacao(listaPontuacoes);
    
    // Descarrega textura do boss
    UnloadBossTexture();
    
    // Encerramento
    CloseAudioDevice();
    CloseWindow();
    return 0;
}
