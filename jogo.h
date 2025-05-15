// jogo.h
#ifndef JOGO_H
#define JOGO_H

#include "raylib.h"

// Constantes do jogo
#define LARGURA_TELA 800
#define ALTURA_TELA 600
#define AREA_JOGO_X 50
#define AREA_JOGO_Y 160  // Move a área do jogo para baixo para exibir o quadrado do boss
#define AREA_JOGO_LARGURA 700
#define AREA_JOGO_ALTURA 400  // Reduzido para abrir espaço para o quadrado do boss
#define TAMANHO_CORACAO 32
#define INTERVALO_GERACAO_OBSTACULO 20  // Reduzido para gerar obstáculos mais rápido
#define PONTOS_POR_FRAME 0.5f  // Aumentado para pontuação mais rápida

// Constantes de obstáculos - aumentados para maior dificuldade
#define MAX_OBSTACULOS_BRANCOS 70  // Aumentado de 50 para 70
#define MAX_OBSTACULOS_ROXOS 45    // Aumentado de 30 para 45
#define MAX_OBSTACULOS_AMARELOS 15 // Aumentado de 10 para 15

// Velocidades - aumentadas para maior dificuldade
#define VELOCIDADE_BASE_OBSTACULO 5.0f  // Aumentado para maior velocidade
#define VELOCIDADE_CORACAO 6.5f  // Aumentado para maior velocidade

// Constantes para bosses
#define MAX_BOSSES 3
#define MAX_PADROES_ATAQUE 5
#define MAX_PROJETEIS 50
#define ALTURA_QUADRADO_BOSS 100
#define MARGEM_BOSS 10

// Constantes para números de dano
#define MAX_NUMEROS_DANO 10  // Máximo de números de dano na tela ao mesmo tempo

// Níveis de dificuldade
typedef enum {
    FACIL,
    NORMAL,
    DIFICIL,
    CUPHEAD  // Modo extremamente difícil
} NivelDificuldade;

// Tipos de padrões de ataque
typedef enum {
    PADRAO_CIRCULAR,
    PADRAO_ESPIRAL,
    PADRAO_PERSEGUICAO,
    PADRAO_ALEATORIO,
    PADRAO_ONDAS
} PadraoAtaque;

// Estrutura para os obstáculos
typedef struct {
    Vector2 posicao;
    float velocidade;
    float angulo;
    bool ativo;
    int tipo; // 0 = branco, 1 = roxo, 2 = amarelo
    float escala; // Para variação de tamanho
    float rotacao; // Para rotação dinâmica
    int comprimento; // Comprimento do obstáculo
} Obstaculo;

// Estrutura para os projéteis dos bosses
typedef struct {
    Vector2 posicao;
    Vector2 velocidade;
    float raio;
    Color cor;
    bool ativo;
    Texture2D textura;  // Textura do projétil
    bool usaSprite;     // Indica se deve usar sprite em vez de círculo
    float rotacao;      // Rotação do sprite
    float escala;       // Escala do sprite
} Projetil;

// Função para verificar colisão de projétil
int verificarColisaoProjetil(Vector2 posicaoCoracao, Projetil projetil);

// Estrutura para os bosses
typedef struct {
    Vector2 posicao;
    float vida;
    float vidaMaxima;
    int fase;
    bool ativo;
    float tempo;
    float tempoAtaque;
    float tempoEntreAtaques;
    PadraoAtaque padraoAtual;
    Texture2D textura;
    Rectangle frameAtual;
    int numFrames;
    float tempoFrame;
    char nome[50];
    char descricao[200];
    Projetil projeteis[MAX_PROJETEIS];
} Boss;

// Estrutura para os números de dano/cura flutuantes
typedef struct {
    Vector2 posicao;
    Vector2 velocidade;
    float valor;
    float tempo;
    bool ativo;
    bool ehDano; // true = dano, false = cura
    Color cor;
} NumeroDano;

// Variáveis globais compartilhadas entre arquivos
extern Vector2 posicaoCoracao;
extern float vidaCoracao;
extern float pontuacao;
extern int faseAtual;
extern NivelDificuldade dificuldadeAtual;

// Variáveis para o efeito visual de dano (usadas também em boss.c)
extern float efeitoDanoTempo;
extern float ultimoDano;

// Funções de inicialização
void inicializarJogo(void);
void inicializarCoracao(void);
void inicializarObstaculos(void);
void inicializarBosses(void);

// Funções de finalização
void reiniciarJogo(void);
void finalizarJogo(void);

// Funções de atualização
bool atualizarJogo(void);
void atualizarCoracao(void);
void atualizarObstaculos(void);
void atualizarObstaculosBrancos(void);
void atualizarObstaculosRoxos(void);
void atualizarObstaculosAmarelos(void);
void atualizarBosses(void);
void atualizarProjeteis(void);
void atualizarFase(void);

// Funções para gerar obstáculos
void gerarObstaculoBranco(void);
void gerarObstaculoRoxo(void);
void gerarObstaculoAmarelo(void);

// Funções para mudança de fase
void mudarParaFase2(void);
void mudarParaFase3(void);

// Funções de colisão
bool detectarColisoes(void);

// Funções de números de dano
void inicializarNumerosDano(void);
void adicionarNumeroDano(float valor, Vector2 posicao, bool ehDano);
void atualizarNumerosDano(void);
void desenharNumerosDano(void);

// Funções de desenho
void desenharJogo(void);
void desenharCoracao(void);
void desenharObstaculos(void);
void desenharBarra(void);
void desenharBoss(void);
void desenharBosses(void);
void desenharProjeteis(void);
void desenharFase(void);

// Declaração da função definirDificuldade
void definirDificuldade(NivelDificuldade nivel);

// Declaração da função ativarBossDaFase
void ativarBossDaFase(int fase);

#endif // JOGO_H
