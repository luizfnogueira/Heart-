// jogo.h
#ifndef JOGO_H
#define JOGO_H

#include "raylib.h"
#include "charada.h"  // Incluir o sistema de charadas

// Constantes do jogo
#define LARGURA_TELA 800
#define ALTURA_TELA 600
#define AREA_JOGO_LARGURA 240
#define AREA_JOGO_ALTURA 240
#define AREA_JOGO_X ((LARGURA_TELA - AREA_JOGO_LARGURA) / 2)
#define AREA_JOGO_Y 300
#define TAMANHO_CORACAO 16
#define INTERVALO_GERACAO_OBSTACULO 20
#define PONTOS_POR_FRAME 0.5f

// Constantes de obstáculo
#define MAX_OBSTACULOS_BRANCOS 70
#define VELOCIDADE_CORACAO 6.5f

// Constantes para bosses
#define MAX_BOSSES 3
#define MAX_PADROES_ATAQUE 5
#define MAX_PROJETEIS 50
#define ALTURA_QUADRADO_BOSS 240
#define MARGEM_BOSS 10

// Níveis de dificuldade
typedef enum {
    FACIL,
    NORMAL,
    DIFICIL,
    CUPHEAD  // Modo extremamente difícil
} NivelDificuldade;

// Estruturas
typedef struct {
    Vector2 posicao;
    float velocidade;
    bool ativo;
    int comprimento;
    float tempoAnimacao;
} Obstaculo;

typedef struct {
    Vector2 posicao;
    Vector2 velocidade;
    float raio;
    Color cor;
    bool ativo;
    Texture2D textura;
    bool usaSprite;
    float rotacao;
    float escala;
} Projetil;

typedef struct {
    Vector2 posicao;
    float vida;
    float vidaMaxima;
    int fase;
    bool ativo;
    float tempo;
    float tempoAtaque;
    float tempoEntreAtaques;
    int padraoAtual;
    Texture2D textura;
    Rectangle frameAtual;
    int numFrames;
    float tempoFrame;
    char nome[50];
    char descricao[200];
    Projetil projeteis[MAX_PROJETEIS];
} Boss;

// Definições adicionais
#define MAX_NUMEROS_DANO 50

// Estrutura para números de dano flutuantes
typedef struct {
    Vector2 posicao;
    Vector2 velocidade;
    float valor;
    float tempo;
    bool ativo;
    bool ehDano;
    Color cor;
} NumeroDano;

// Variáveis globais
extern float pontuacao;

// Funções essenciais
void inicializarJogo(void);
bool atualizarJogo(void);
void desenharJogo(void);
void definirDificuldade(NivelDificuldade nivel);
void ativarBossDaFase(int fase);

// Funções adicionais
void inicializarObstaculos(void);
void inicializarNumerosDano(void);
void inicializarCoracao(void);
void atualizarCoracao(void);
void atualizarObstaculos(void);
void atualizarFase(void);
bool detectarColisoes(void);
void mudarParaFase2(void);
void mudarParaFase3(void);
void atualizarNumerosDano(void);
void desenharNumerosDano(void);
void desenharObstaculos(void);
void desenharProjeteis(void);
void atualizarProjeteis(void);
void atualizarBosses(void);
void desenharCoracao(void);
void inicializarBosses(void);
void desenharBosses(void);

#endif // JOGO_H
