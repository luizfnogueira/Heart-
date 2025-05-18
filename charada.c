#include "charada.h"
#include "jogo.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <curl/curl.h>
#include <ctype.h>
#include "raylib.h"

// Referências externas às variáveis e funções do jogo.c
extern int faseAtual;
extern void mudarParaFase2(void);
extern void mudarParaFase3(void);

// Variáveis globais
Charada charadaAtual;
bool modoCharada = false;
static Texture2D fundoCharada;
static char mensagemStatus[256] = "";
static float tempoFeedback = 0.0f;
static Rectangle caixaInput = {0};
static int framesCounter = 0;

// Função para gerar charada usando Gemini API
void gerarNovaCharada(int fase) {
    // Inicializar estado da charada
    static int ultimaCharada[3] = {-1, -1, -1}; // Último índice usado para cada fase

    charadaAtual.tentativasRestantes = 3;
    charadaAtual.ativa = true;
    charadaAtual.respondida = false;
    charadaAtual.acertou = false;
    charadaAtual.faseAlvo = fase;
    charadaAtual.tentativaAtual[0] = '\0';

    // Charadas pré-definidas com múltiplas respostas
    const char* charadasFase1[][4] = {
        {"Sou veloz como o vento, mas não tenho corpo. Posso te machucar, mas nunca te tocar. O que sou eu?", "obstáculo", "vento", "fogo"},
        {"Em um mundo digital, sou a luz que se move. Vermelho como sangue, mas não estou vivo. O que sou eu?", "coração", "sangue", "luz"},
        {"Quanto mais me moverem, mais forte fico. Quando me tocam, causo dor. O que sou eu?", "osso", "pedra", "metal"}
    };

    const char* charadasFase2[][4] = {
        {"Estou sempre correndo, mas nunca chego a lugar nenhum. O que sou eu?", "jogo", "tempo", "vento"},
        {"Sou invisível até te ferir. Depois disso, não posso mais te machucar. O que sou eu?", "armadilha", "sombra", "vento"},
        {"Em um labirinto vermelho, me escondo nas sombras. Quando me encontrarem, já será tarde demais. O que sou eu?", "perigo", "medo", "sombra"}
    };

    const char* charadasFase3[][4] = {
        {"Quanto mais rápido você corre, mais eu me aproximo. O que sou eu?", "destino", "tempo", "sombra"},
        {"Sou feito de memórias e pesadelos. Toda vez que me enfrenta, fico mais forte. O que sou eu?", "medo", "sombra", "dor"},
        {"No centro de tudo, pulsando sem parar, sou seu maior aliado e seu pior inimigo. O que sou eu?", "coração", "vida", "tempo"}
    };

    const char* (*charadas)[4];
    int numCharadas;

    switch (fase) {
        case 1:
            charadas = charadasFase1;
            numCharadas = 3;
            break;
        case 2:
            charadas = charadasFase2;
            numCharadas = 3;
            break;
        case 3:
        default:
            charadas = charadasFase3;
            numCharadas = 3;
            break;
    }

    // Escolher charada aleatória
    int indice;
    do {
        indice = GetRandomValue(0, numCharadas - 1);
    } while (indice == ultimaCharada[fase - 1] && numCharadas > 1);

    ultimaCharada[fase - 1] = indice;

    strcpy(charadaAtual.pergunta, charadas[indice][0]);
    strcpy(charadaAtual.resposta, charadas[indice][1]);

    // Armazenar alternativas
    strcpy(charadaAtual.alternativas[0], charadas[indice][1]); // Resposta correta
    strcpy(charadaAtual.alternativas[1], charadas[indice][2]);
    strcpy(charadaAtual.alternativas[2], charadas[indice][3]);

    // Embaralhar alternativas
    for (int i = 0; i < 3; i++) {
        int j = GetRandomValue(0, 2);
        char temp[64];
        strcpy(temp, charadaAtual.alternativas[i]);
        strcpy(charadaAtual.alternativas[i], charadaAtual.alternativas[j]);
        strcpy(charadaAtual.alternativas[j], temp);
    }
}

// Inicializar sistema de charadas
void inicializarSistemaCharadas(void) {
    // Carregar textura de fundo com cor sólida (em vez de gradiente)
    Image img = GenImageColor(GetScreenWidth(), GetScreenHeight(), 
                            (Color){20, 10, 40, 255});
    fundoCharada = LoadTextureFromImage(img);
    UnloadImage(img);
    
    // Inicializar posição da caixa de input
    caixaInput = (Rectangle){
        GetScreenWidth()/2 - 200,
        GetScreenHeight()/2 + 50,
        400,
        40
    };
    
    // Inicializar CURL para requisições HTTP
    curl_global_init(CURL_GLOBAL_ALL);
    
    // Inicializar primeira charada
    memset(&charadaAtual, 0, sizeof(Charada));
}

// Liberar recursos
void liberarRecursosCharada(void) {
    UnloadTexture(fundoCharada);
    curl_global_cleanup();
}

// Verificar se a resposta está correta
bool verificarResposta(void) {
    // Converter tentativa para minúsculas
    char tentativaLower[128] = {0};
    strcpy(tentativaLower, charadaAtual.tentativaAtual);
    for(int i = 0; tentativaLower[i]; i++) {
        tentativaLower[i] = tolower(tentativaLower[i]);
    }
    
    // Verificar se contém a palavra-chave da resposta
    if(strstr(tentativaLower, charadaAtual.resposta)) {
        charadaAtual.acertou = true;
        charadaAtual.respondida = true;
        strcpy(mensagemStatus, "CORRETO! Avançando para a próxima fase...");
        tempoFeedback = 2.0f;
        return true;
    } else {
        charadaAtual.tentativasRestantes--;
        if(charadaAtual.tentativasRestantes <= 0) {
            charadaAtual.respondida = true;
            strcpy(mensagemStatus, "GAME OVER! Você esgotou suas tentativas.");
            tempoFeedback = 2.0f;
        } else {
            sprintf(mensagemStatus, "Incorreto! Tentativas restantes: %d", charadaAtual.tentativasRestantes);
            tempoFeedback = 2.0f;
        }
        return false;
    }
}

// Atualizar a tela de charada
bool atualizarTelaCharada(void) {
    if (!modoCharada) return false;

    framesCounter++;

    // Atualizar temporizador de feedback
    if (tempoFeedback > 0) {
        tempoFeedback -= GetFrameTime();
        if (tempoFeedback <= 0 && charadaAtual.respondida) {
            if (charadaAtual.acertou) {
                modoCharada = false;
                // Avançar para a próxima fase
                if (charadaAtual.faseAlvo == 2) {
                    faseAtual = 2;
                    mudarParaFase2();
                } else if (charadaAtual.faseAlvo == 3) {
                    faseAtual = 3;
                    mudarParaFase3();
                } else if (charadaAtual.faseAlvo == 4) {
                    // Vitória do jogo
                    return false; // Jogo vencido
                }
                return true; // Continua o jogo
            } else if (charadaAtual.tentativasRestantes <= 0) {
                modoCharada = false;
                return false; // Game Over
            }
        }
    }

    // Processar cliques nos botões de alternativas
    if (!charadaAtual.respondida) {
        int buttonWidth = 200;
        int buttonHeight = 50;
        int buttonY = GetScreenHeight() / 2 + 20;

        for (int i = 0; i < 3; i++) {
            int buttonX = GetScreenWidth() / 2 - buttonWidth / 2;
            Rectangle buttonRect = {buttonX, buttonY + i * (buttonHeight + 10), buttonWidth, buttonHeight};

            // Verificar se o botão foi clicado
            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                Vector2 mousePos = GetMousePosition();
                if (CheckCollisionPointRec(mousePos, buttonRect)) {
                    // Verificar se a resposta está correta
                    if (strcmp(charadaAtual.alternativas[i], charadaAtual.resposta) == 0) {
                        charadaAtual.acertou = true;
                        strcpy(mensagemStatus, "CORRETO! Avançando para a próxima fase...");
                    } else {
                        charadaAtual.tentativasRestantes--;
                        if (charadaAtual.tentativasRestantes <= 0) {
                            strcpy(mensagemStatus, "GAME OVER! Você esgotou suas tentativas.");
                        } else {
                            sprintf(mensagemStatus, "Incorreto! Tentativas restantes: %d", charadaAtual.tentativasRestantes);
                        }
                    }
                    charadaAtual.respondida = true;
                    tempoFeedback = 2.0f;
                    break;
                }
            }
        }
    }

    // Escape para voltar ao menu (opcional)
    if (IsKeyPressed(KEY_ESCAPE)) {
        modoCharada = false;
        return false;
    }

    return true; // Continuando no modo charada
}

// Desenhar a tela de charada
void desenharTelaCharada(void) {
    if (!modoCharada) return;

    // Desenhar fundo
    DrawTexture(fundoCharada, 0, 0, WHITE);

    // Desenhar título
    char titulo[64];
    sprintf(titulo, "CHARADA DA FASE %d", charadaAtual.faseAlvo);
    int tituloWidth = MeasureText(titulo, 40);
    DrawText(titulo, GetScreenWidth() / 2 - tituloWidth / 2, 80, 40, GOLD);

    // Desenhar pergunta da charada
    DrawRectangle(GetScreenWidth() / 2 - 350, GetScreenHeight() / 2 - 150, 700, 150, (Color){0, 0, 0, 200});
    DrawRectangleLines(GetScreenWidth() / 2 - 350, GetScreenHeight() / 2 - 150, 700, 150, GOLD);

    int fontSize = 24;
    int startY = GetScreenHeight() / 2 - 130;
    DrawText(charadaAtual.pergunta, GetScreenWidth() / 2 - MeasureText(charadaAtual.pergunta, fontSize) / 2, startY, fontSize, WHITE);

    // Desenhar botões de alternativas
    int buttonWidth = 200;
    int buttonHeight = 50;
    int buttonY = GetScreenHeight() / 2 + 20;
    for (int i = 0; i < 3; i++) {
        int buttonX = GetScreenWidth() / 2 - buttonWidth / 2;
        DrawRectangle(buttonX, buttonY + i * (buttonHeight + 10), buttonWidth, buttonHeight, DARKGRAY);
        DrawRectangleLines(buttonX, buttonY + i * (buttonHeight + 10), buttonWidth, buttonHeight, WHITE);
        DrawText(charadaAtual.alternativas[i], buttonX + 10, buttonY + i * (buttonHeight + 10) + 15, 20, WHITE);
    }

    // Removida a caixa de input e o texto digitado

    // Desenhar instruções
    int textWidth = MeasureText("Escolha a resposta correta clicando no botão", 20);
    DrawText("Escolha a resposta correta clicando no botão",
             GetScreenWidth() / 2 - textWidth / 2,
             GetScreenHeight() - 70, 20, LIGHTGRAY);

    // Desenhar mensagem de status
    if (tempoFeedback > 0) {
        Color statusColor = WHITE;
        if (strstr(mensagemStatus, "CORRETO")) statusColor = GREEN;
        else if (strstr(mensagemStatus, "GAME OVER")) statusColor = RED;
        else if (strstr(mensagemStatus, "Incorreto")) statusColor = ORANGE;

        textWidth = MeasureText(mensagemStatus, 30);
        DrawText(mensagemStatus, GetScreenWidth() / 2 - textWidth / 2, GetScreenHeight() - 120, 30, statusColor);
    }

    // Desenhar contagem de tentativas
    char tentativas[32];
    sprintf(tentativas, "Tentativas: %d", charadaAtual.tentativasRestantes);
    textWidth = MeasureText(tentativas, 20);
    DrawText(tentativas, GetScreenWidth() - textWidth - 20, 20, 20, LIME);
}
