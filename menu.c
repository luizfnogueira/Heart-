#include "raylib.h"
#include "menu.h"
#include "pontuacao.h"
#include <string.h>
#include <stdio.h>
#include <math.h>

// Definições de constantes
#define LARGURA_TELA 800
#define ALTURA_TELA 600
#define OPCOES_MENU 5

// Variáveis globais do menu
static int opcaoSelecionada = 0;
static const char *opcoesMenu[OPCOES_MENU] = { 
    "Iniciar Jogo", 
    "História", 
    "Créditos", 
    "Ranking", 
    "Sair" 
};

// Funções do menu principal
int atualizarMenu(void) {
    // Controle de navegação do menu
    if (IsKeyPressed(KEY_DOWN)) {
        opcaoSelecionada = (opcaoSelecionada + 1) % OPCOES_MENU;
    }
    if (IsKeyPressed(KEY_UP)) {
        opcaoSelecionada = (opcaoSelecionada - 1 + OPCOES_MENU) % OPCOES_MENU;
    }

    // Verificação de seleção de opção
    if (IsKeyPressed(KEY_ENTER)) {
        switch (opcaoSelecionada) {
            case 0: return TELA_JOGO;      // Iniciar Jogo
            case 1: return TELA_HISTORIA;  // História
            case 2: return TELA_CREDITOS;  // Créditos
            case 3: return TELA_RANKING;   // Ranking
            case 4: return TELA_SAIR;      // Sair
        }
    }
    
    return TELA_MENU; // Permanece no menu se nenhuma opção for selecionada
}

void desenharMenu(void) {
    // Desenha o título do jogo com sombra
    DrawText("💜 HEART! 💜", LARGURA_TELA/2 - MeasureText("💜 HEART! 💜", 60)/2 + 2, 82, 60, DARKPURPLE);
    DrawText("💜 HEART! 💜", LARGURA_TELA/2 - MeasureText("💜 HEART! 💜", 60)/2, 80, 60, RED);
    
    // Desenha o coração animado pulsante
    static float tempo = 0.0f;
    tempo += GetFrameTime() * 2.0f;
    float escala = 1.0f + sinf(tempo) * 0.1f;  // Efeito pulsante
    
    int tamanhoCoracao = (int)(40.0f * escala);
    Color corCoracao = (Color){
        200 + (int)(sinf(tempo) * 55.0f),  // Vermelho pulsante
        0,
        50 + (int)(sinf(tempo + 2.0f) * 50.0f),  // Azul variando
        255
    };
    
    DrawText("   ♥   ", LARGURA_TELA/2 - 30, 160, tamanhoCoracao, corCoracao);
    // Desenha as opções do menu
    for (int i = 0; i < OPCOES_MENU; i++) {
        Color cor = (i == opcaoSelecionada) ? MAGENTA : WHITE;
        DrawText(opcoesMenu[i], 
                 LARGURA_TELA/2 - MeasureText(opcoesMenu[i], 30)/2, 
                 250 + i * 50, 
                 30, 
                 cor);
    }
    
    // Instruções de navegação
    DrawText("Use as setas para navegar e ENTER para selecionar", 
             LARGURA_TELA/2 - MeasureText("Use as setas para navegar e ENTER para selecionar", 20)/2, 
             ALTURA_TELA - 40, 
             20, 
             LIGHTGRAY);
}

// Funções da tela de créditos
int atualizarTelaCreditos(void) {
    if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_ESCAPE)) {
        return TELA_MENU;
    }
    return TELA_CREDITOS;
}

void desenharTelaCreditos(void) {
    DrawText("CRÉDITOS", LARGURA_TELA/2 - MeasureText("CRÉDITOS", 40)/2, 80, 40, PURPLE);
    
    DrawText("Desenvolvido por:", LARGURA_TELA/2 - MeasureText("Desenvolvido por:", 30)/2, 160, 30, WHITE);
    DrawText("Thiago Queiroz", LARGURA_TELA/2 - MeasureText("Thiago Queiroz", 25)/2, 220, 25, LIGHTGRAY);
    DrawText("Lucas Rodrigues", LARGURA_TELA/2 - MeasureText("Lucas Rodrigues", 25)/2, 260, 25, LIGHTGRAY);
    DrawText("Luiz Nogueira", LARGURA_TELA/2 - MeasureText("Luiz Nogueira", 25)/2, 300, 25, LIGHTGRAY);
    
    DrawText("Reimplementação usando Raylib", LARGURA_TELA/2 - MeasureText("Reimplementação usando Raylib", 20)/2, 360, 20, GRAY);
    
    DrawText("Pressione ENTER ou ESC para voltar ao menu", 
             LARGURA_TELA/2 - MeasureText("Pressione ENTER ou ESC para voltar ao menu", 20)/2, 
             ALTURA_TELA - 40, 
             20, 
             LIGHTGRAY);
}

// Funções da tela de história
int atualizarTelaHistoria(void) {
    if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_ESCAPE)) {
        return TELA_MENU;
    }
    return TELA_HISTORIA;
}

void desenharTelaHistoria(void) {
    DrawText("HISTÓRIA", LARGURA_TELA/2 - MeasureText("HISTÓRIA", 40)/2, 40, 40, PURPLE);
    
    const char *historia[] = {
        "Em um mundo onde a luz se apagou,",
        "você é a última centelha de um",
        "coração que já foi cheio de vida.",
        "",
        "Preso em uma dimensão entre o vazio",
        "e a esperança, você enfrenta os",
        "ecos sombrios dos medos e",
        "arrependimentos de uma alma perdida.",
        "",
        "Cada passo que você dá, cada",
        "obstáculo que supera, é uma luta",
        "para recuperar fragmentos de",
        "sentimentos e memórias esquecidas.",
        "",
        "Mas cuidado, pois se o HP do",
        "coração chegar a zero, ele se",
        "despedaçará, e toda esperança será",
        "perdida para sempre."
    };
    
    for (int i = 0; i < 18; i++) {
        DrawText(historia[i], LARGURA_TELA/2 - MeasureText(historia[i], 20)/2, 100 + i * 25, 20, WHITE);
    }
    
    DrawText("Pressione ENTER ou ESC para voltar ao menu", 
             LARGURA_TELA/2 - MeasureText("Pressione ENTER ou ESC para voltar ao menu", 20)/2, 
             ALTURA_TELA - 40, 
             20, 
             LIGHTGRAY);
}

// Variáveis globais para compartilhamento entre funções
char nomeJogador[50] = "";
static int pontuacaoFinal = 0;
static bool nomeInserido = false;

// Função para definir a pontuação final quando o jogador morre
void definirPontuacaoFinal(int pontuacao) {
    pontuacaoFinal = pontuacao;
    nomeInserido = false;
    nomeJogador[0] = '\0'; // Limpa o nome para nova entrada
}

// Funções da tela de game over
int atualizarTelaGameOver(ListaPontuacao* listaPontuacoes) {
      // Se o nome ainda não foi inserido, captura entrada do usuário
    if (!nomeInserido) {
        int tecla = GetCharPressed();
        
        // Verifica se alguma tecla foi pressionada
        while (tecla > 0) {            // Apenas caracteres ASCII válidos
            if ((tecla >= 32) && (tecla <= 125) && (strlen(nomeJogador) < 49)) {
                int len = strlen(nomeJogador);
                nomeJogador[len] = (char)tecla;
                nomeJogador[len + 1] = '\0'; // Posiciona o terminador corretamente
            }
            tecla = GetCharPressed();
        }
        
        // Permite apagar caracteres
        if (IsKeyPressed(KEY_BACKSPACE)) {
            int tamanhoNome = strlen(nomeJogador);
            if (tamanhoNome > 0) {
                nomeJogador[tamanhoNome-1] = '\0';
            }
        }
        
        // Confirma o nome quando ENTER é pressionado
        if (IsKeyPressed(KEY_ENTER) && (strlen(nomeJogador) > 0)) {
            nomeInserido = true;
            adicionarPontuacao(listaPontuacoes, nomeJogador, pontuacaoFinal);
            salvarPontuacoes(listaPontuacoes);
        }    } else {
        // Após inserir o nome, aguarda ENTER para voltar ao menu
        if (IsKeyPressed(KEY_ENTER)) {
            // Reinicia as variáveis estáticas para próximo uso
            nomeJogador[0] = '\0';
            nomeInserido = false;
            return TELA_MENU;
        }
    }
    
    return TELA_GAMEOVER;
}

void desenharTelaGameOver(void) {
    static float tempo = 0.0f;
    tempo += GetFrameTime();
    
    // Efeito de pulso no texto Game Over
    float escala = 1.0f + sinf(tempo * 2.0f) * 0.05f;
    int tamanhoFonte = (int)(60.0f * escala);
    
    // Efeito de sangue escorrendo
    DrawText("GAME OVER", 
             LARGURA_TELA/2 - MeasureText("GAME OVER", tamanhoFonte)/2, 
             100, 
             tamanhoFonte, 
             (Color){200 + (int)(sinf(tempo) * 55.0f), 0, 0, 255});
             
    // Efeito de sangue pingando
    for(int i = 0; i < 5; i++) {
        float offsetY = fabsf(sinf(tempo * 0.5f + i)) * 20.0f;
        DrawCircle(LARGURA_TELA/2 - 100 + i * 50, 
                  180 + (int)offsetY, 
                  3.0f, 
                  RED);
    }
    
    DrawText("A última centelha se apagou no vazio...", 
             LARGURA_TELA/2 - MeasureText("A última centelha se apagou no vazio...", 30)/2, 
             200, 
             30, 
             WHITE);
      DrawText("'Mesmo nas trevas, a luz pode renascer.'", 
             LARGURA_TELA/2 - MeasureText("'Mesmo nas trevas, a luz pode renascer.'", 25)/2, 
             250, 
             25, 
             LIGHTGRAY);
    
    // Exibe a pontuação final do jogador
    DrawText(TextFormat("Pontuação final: %d", pontuacaoFinal),
             LARGURA_TELA/2 - MeasureText(TextFormat("Pontuação final: %d", pontuacaoFinal), 30)/2,
             290,
             30,
             GOLD);
    
    DrawText("Digite seu nome para salvar a pontuação:", 
             LARGURA_TELA/2 - MeasureText("Digite seu nome para salvar a pontuação:", 25)/2, 
             330, 
             25, 
             WHITE);    // Campo para digitação do nome com destaque
    Color corFundo = (((int)(GetTime() * 1.5f) % 2) == 0 && !nomeInserido) ? 
                    (Color){70, 70, 70, 255} : DARKGRAY;
    DrawRectangle(LARGURA_TELA/2 - 150, 370, 300, 40, corFundo);
    DrawRectangleLines(LARGURA_TELA/2 - 150, 370, 300, 40, LIGHTGRAY);
    
    // Cursor piscante
    if (!nomeInserido) {
        int tamanhoTexto = MeasureText(nomeJogador, 20);
        if (((int)(GetTime() * 2) % 2) == 0) {
            DrawRectangle(LARGURA_TELA/2 - 140 + tamanhoTexto, 380, 2, 20, WHITE);
        }
    }
    
    // Desenha o nome sendo digitado
    DrawText(nomeJogador, LARGURA_TELA/2 - 140, 380, 20, WHITE);
      DrawText("Pressione ENTER para confirmar", 
             LARGURA_TELA/2 - MeasureText("Pressione ENTER para confirmar", 20)/2, 
             430, 
             20, 
             LIGHTGRAY);
}
