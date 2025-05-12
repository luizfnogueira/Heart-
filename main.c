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
#include "gemini_config.h"
#include <curl/curl.h>

// Definições de constantes
#define LARGURA_TELA 800
#define ALTURA_TELA 600
#define TITULO_JOGO "Heart!"

// Definição global da música para ser acessível na finalização
Music backgroundMusic;

// Função principal
int main(void) {
    // Inicialização do Raylib
    InitWindow(LARGURA_TELA, ALTURA_TELA, TITULO_JOGO);
    SetTargetFPS(60);
    
    // Inicialização do áudio
    InitAudioDevice();
    
    // Carregamento de recursos
    backgroundMusic = LoadMusicStream("recursos/Condemned Tower - Castlevania Dawn of Sorrow OST.mp3");
    if (backgroundMusic.frameCount == 0) { // Verifica se a música foi carregada
        TraceLog(LOG_WARNING, "Falha ao carregar a música de fundo.");
    } else {
        PlayMusicStream(backgroundMusic);
        SetMusicVolume(backgroundMusic, 0.5f); // Ajuste o volume conforme necessário (0.0 a 1.0)
    }
    
    // Inicialização de variáveis do jogo
    int telaAtual = TELA_MENU;
    ListaPontuacao* listaPontuacoes = inicializarListaPontuacao();
    carregarPontuacoes(listaPontuacoes);
    
    // Inicialização da API Gemini para controle de bosses por IA
    ConfiguracaoGemini configGemini;
    inicializarConfigGemini(&configGemini);
    
    // Valida a configuração da API Gemini
    if (!validarConfigGemini(configGemini)) {
        printf("AVISO: Configuração da API Gemini inválida. Os bosses usarão comportamento padrão.\n");
    } else {
        printf("API Gemini configurada com sucesso! Os bosses serão controlados por IA.\n");
        printf("Email de contato do projeto: %s\n", EMAIL_CONTATO);
    }
    
    // Inicializa a biblioteca CURL para requisições HTTP
    curl_global_init(CURL_GLOBAL_ALL);
    
    // Exibe informações sobre o projeto acadêmico
    exibirInformacoesProjetoAcademico();
    
    // Loop principal do jogo
    while (!WindowShouldClose()) {
        // Atualização
        if (IsMusicStreamPlaying(backgroundMusic)) {
            UpdateMusicStream(backgroundMusic);
        }
        
        // Lógica baseada na tela atual
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
                    // O jogador morreu, salva a pontuação atual e muda para a tela de game over
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
            case TELA_MENU:
                desenharMenu();
                break;
                
            case TELA_JOGO:
                desenharJogo();
                break;
                
            case TELA_CREDITOS:
                desenharTelaCreditos();
                break;
                
            case TELA_HISTORIA:
                desenharTelaHistoria();
                break;
                
            case TELA_RANKING:
                desenharTelaRanking(listaPontuacoes);
                break;
                
            case TELA_GAMEOVER:
                desenharTelaGameOver();
                break;
        }
        
        EndDrawing();
    }
    
    // Descarregamento de recursos
    UnloadMusicStream(backgroundMusic);
    liberarListaPontuacao(listaPontuacoes);
    
    // Limpeza da biblioteca CURL
    curl_global_cleanup();
    
    // Encerramento do Raylib
    CloseAudioDevice();
    CloseWindow();
    
    return 0;
}
