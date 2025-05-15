#include "jogo.h"
#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include "raylib.h"
#include <string.h>
#include "obstaculo_sprites.h"

// Variáveis globais do jogo
Vector2 posicaoCoracao;
float vidaCoracao = 200.0f;
float pontuacao = 0.0f;
int faseAtual = 1;
NivelDificuldade dificuldadeAtual = NORMAL;
Texture2D texturaCoracao;
Texture2D texturaOssoReto;
Texture2D texturaOssoHorizontal;
// --- Adicionado: textura do osso grande do topo ---
Texture2D texturaOssoGrandeTopo;

// Variáveis locais do jogo
static bool jogadorMovendo;
static int contadorObstaculos = 0;
static float tempoJogo = 0.0f;
static bool modoChefao = false;
static int bossAtual = 0;
static float velocidadeBase = 2.5f;
static float velocidadeCoracao = VELOCIDADE_CORACAO;

// Variáveis para o efeito visual de dano
float efeitoDanoTempo = 0.0f;
float ultimoDano = 0.0f;
static float vidaAnterior = 200.0f;

// Array de números de dano flutuantes
static NumeroDano numerosDano[MAX_NUMEROS_DANO];

// Arrays de obstáculos
static Obstaculo obstaculosBrancos[MAX_OBSTACULOS_BRANCOS];  
static Obstaculo obstaculosRoxos[MAX_OBSTACULOS_ROXOS];    
static Obstaculo obstaculosAmarelos[MAX_OBSTACULOS_AMARELOS];

static bool efeitosVisuaisAvancados = true;

// Implementação das funções do jogo

// Inicializa o jogo
Boss bosses[MAX_BOSSES];

void inicializarJogo(void) {
    // Inicializa variáveis do jogo
    posicaoCoracao.x = AREA_JOGO_X + 50;
    posicaoCoracao.y = AREA_JOGO_Y + AREA_JOGO_ALTURA / 2;
    vidaCoracao = 200.0f;
    pontuacao = 0;
    faseAtual = 1;
    jogadorMovendo = false;
    contadorObstaculos = 0;
    velocidadeBase = 2.5f;
    velocidadeCoracao = VELOCIDADE_CORACAO;
    modoChefao = true;
    dificuldadeAtual = NORMAL;
    bosses[0].ativo = true;

    // Carregar sprite sheet do Undertale (coração)
    texturaCoracao = LoadTexture("recursos/Sprites/Custom Edited - Undertale Customs - Battle Interface.png");
    texturaOssoReto = LoadTexture("recursos/Sprites/ossos.png");
    texturaOssoHorizontal = LoadTexture("recursos/Sprites/ossos.png");
    // --- Carregar textura do osso grande do topo uma vez ---
    texturaOssoGrandeTopo = LoadTexture("recursos/Sprites/PC Computer - Undertale - Sans.png");

    // Inicializa obstáculos
    inicializarObstaculos();
    
    // Inicializa números de dano
    inicializarNumerosDano();
    
    // Inicializa e ativa o boss da fase 1
    inicializarBosses();
    ativarBossDaFase(1);
    
    // Inicializa o coração e obstáculos
    inicializarCoracao();
    inicializarObstaculos();
}

// --- Liberar textura do osso grande do topo ao fechar o jogo ---
void liberarRecursosJogo(void) {
    UnloadTexture(texturaCoracao);
    UnloadTexture(texturaOssoReto);
    UnloadTexture(texturaOssoHorizontal);
    UnloadTexture(texturaOssoGrandeTopo);
}

// Inicializa a posição do coração
void inicializarCoracao(void) {
    posicaoCoracao.x = AREA_JOGO_X + 50;
    posicaoCoracao.y = AREA_JOGO_Y + AREA_JOGO_ALTURA - 50;
}

// Inicializa os obstáculos
void inicializarObstaculos(void) {
    // Inicializa obstáculos brancos
    for (int i = 0; i < 50; i++) {
        obstaculosBrancos[i].ativo = false;
    }
    
    // Inicializa obstáculos roxos
    for (int i = 0; i < 30; i++) {
        obstaculosRoxos[i].ativo = false;
    }
    
    // Inicializa obstáculos amarelos
    for (int i = 0; i < 10; i++) {
        obstaculosAmarelos[i].ativo = false;
    }
}

// Gera ossos brancos de forma dinâmica e aleatória na Fase 1
void gerarObstaculoBrancoAleatorio(void) {
    float escalaVertical = 0.35f;
    for (int i = 0; i < MAX_OBSTACULOS_BRANCOS; i++) {
        if (!obstaculosBrancos[i].ativo) {
            obstaculosBrancos[i].ativo = true;
            obstaculosBrancos[i].posicao.x = AREA_JOGO_X + AREA_JOGO_LARGURA;
            // Sorteia tipo de osso
            bool vertical = (GetRandomValue(0, 1) == 1);
            if (vertical) {
                obstaculosBrancos[i].comprimento = 10; // vertical
                float alturaOsso = 153 * escalaVertical;
                float minY = AREA_JOGO_Y + alturaOsso/2 + 5;
                float maxY = AREA_JOGO_Y + AREA_JOGO_ALTURA - alturaOsso/2 - 5;
                obstaculosBrancos[i].posicao.y = GetRandomValue((int)minY, (int)maxY);
            } else {
                obstaculosBrancos[i].comprimento = 30; // horizontal
                float alturaOsso = 13 * escalaVertical;
                float minY = AREA_JOGO_Y + alturaOsso/2 + 5;
                float maxY = AREA_JOGO_Y + AREA_JOGO_ALTURA - alturaOsso/2 - 5;
                obstaculosBrancos[i].posicao.y = GetRandomValue((int)minY, (int)maxY);
            }
            obstaculosBrancos[i].velocidade = velocidadeBase * (1.5f + GetRandomValue(0, 10) / 10.0f);
            break;
        }
    }
}

// Gera uma onda de ossos alinhados, estilo Sans
void gerarOndaDeOssos(void) {
    int quantidade = 8;
    float escalaVertical = 0.35f;
    float alturaOsso = 0;
    float espacamento = 0;
    float yAtual = AREA_JOGO_Y + 20;
    for (int i = 0; i < quantidade; i++) {
        for (int j = 0; j < MAX_OBSTACULOS_BRANCOS; j++) {
            if (!obstaculosBrancos[j].ativo) {
                obstaculosBrancos[j].ativo = true;
                obstaculosBrancos[j].posicao.x = AREA_JOGO_X + AREA_JOGO_LARGURA;
                // Alterna entre osso horizontal e vertical
                if (i % 2 == 0) {
                    obstaculosBrancos[j].comprimento = 30; // horizontal
                    alturaOsso = 13 * escalaVertical;
                } else {
                    obstaculosBrancos[j].comprimento = 10; // vertical
                    alturaOsso = 153 * escalaVertical;
                }
                // Espaçamento mínimo de 10px entre ossos
                espacamento = alturaOsso + 10;
                obstaculosBrancos[j].posicao.y = yAtual + alturaOsso / 2;
                yAtual += espacamento;
                obstaculosBrancos[j].velocidade = velocidadeBase * 1.7f;
                break;
            }
        }
    }
}

// Atualiza o estado do jogo
static float tempoUltimaOnda = 0.0f;
static float tempoUltimoObstaculo = 0.0f;
bool atualizarJogo(void) {
    // Verifica transições de fase
    if (pontuacao >= 200 && faseAtual == 1) {
        faseAtual = 2;
        mudarParaFase2();
    }
    
    if (pontuacao >= 400 && faseAtual == 2) {
        faseAtual = 3;
        mudarParaFase3();
    }
    
    if (pontuacao >= 1000 && faseAtual == 3) {
        return false; // Jogo vencido
    }
    
    // Atualiza o coração
    atualizarCoracao();
    
    // Gera e atualiza obstáculos com frequência aumentada baseada na fase
    contadorObstaculos++;
    int intervaloAjustado = INTERVALO_GERACAO_OBSTACULO - (faseAtual * 5);
    if (intervaloAjustado < 5) intervaloAjustado = 5;
    
    float tempoAtual = GetTime();
    if (faseAtual == 1) {
        // Fase 1: ossos surgem aleatoriamente
        float intervalo = 0.5f + (GetRandomValue(0, 10) / 20.0f); // 0.5s a 1s
        if (tempoAtual - tempoUltimoObstaculo > intervalo) {
            gerarObstaculoBrancoAleatorio();
            tempoUltimoObstaculo = tempoAtual;
        }
    } else if (tempoAtual - tempoUltimaOnda > 1.2f) {
        gerarOndaDeOssos();
        tempoUltimaOnda = tempoAtual;
    }
    
    // Atualiza os obstáculos
    atualizarObstaculos();
    
    // Atualiza os bosses
    atualizarBosses();
    
    // Atualiza os projéteis
    atualizarProjeteis();
    
    // Detecta colisões
    bool colisao = detectarColisoes();
    
    // Incrementa a pontuação com o tempo
    pontuacao += PONTOS_POR_FRAME;
    
    // Verifica se é hora de ativar o boss baseado na pontuação
    if (!colisao && ((faseAtual == 1 && pontuacao >= 1000) ||
        (faseAtual == 2 && pontuacao >= 3000) ||
        (faseAtual == 3 && pontuacao >= 6000))) {
        
        modoChefao = true;
        bossAtual = faseAtual - 1;
        ativarBossDaFase(faseAtual);
    }
    
    // Atualiza a fase com base na pontuação
    atualizarFase();
    
    // Atualiza o efeito visual de dano
    if (efeitoDanoTempo > 0.0f) {
        efeitoDanoTempo -= GetFrameTime();
        if (efeitoDanoTempo < 0.0f) {
            efeitoDanoTempo = 0.0f;
        }
    }
    
    // Atualiza os números de dano
    atualizarNumerosDano();
    
    // Verifica se o jogador perdeu
    if (vidaCoracao <= 0) {
        return false;
    }
    
    // Teclas especiais
    if (IsKeyPressed(KEY_F1)) {
        dificuldadeAtual = (dificuldadeAtual + 1) % 4;
        definirDificuldade(dificuldadeAtual);
    }
    
    if (IsKeyPressed(KEY_F2)) {
        efeitosVisuaisAvancados = !efeitosVisuaisAvancados;
    }
    
    return true;
}

void definirDificuldade(NivelDificuldade nivel) {
    dificuldadeAtual = nivel;
}

// Fase 2: mecânica de chão estilo Undertale
void atualizarCoracao(void) {
    if (faseAtual == 2) {
        // Mecânica de chão: só move na horizontal, pode pular
        static float velocidadeY = 0;
        static bool noChao = true;
        float gravidade = 1.2f;
        float forcaPulo = -13.0f;
        float chaoY = AREA_JOGO_Y + AREA_JOGO_ALTURA - 20; // chão
        // --- Limite superior pelo osso grande do topo ---
        float escalaOssoTopo = (float)AREA_JOGO_LARGURA / (12.0f * 6.0f);
        float alturaOsso = 202 * escalaOssoTopo;
        float limiteSuperior = AREA_JOGO_Y + alturaOsso - 10.0f; // -10 para folga visual
        // Movimento horizontal
        if (IsKeyDown(KEY_A) && posicaoCoracao.x > AREA_JOGO_X + TAMANHO_CORACAO/2) {
            posicaoCoracao.x -= velocidadeCoracao;
            jogadorMovendo = true;
        }
        if (IsKeyDown(KEY_D) && posicaoCoracao.x < AREA_JOGO_X + AREA_JOGO_LARGURA - TAMANHO_CORACAO/2) {
            posicaoCoracao.x += velocidadeCoracao;
            jogadorMovendo = true;
        }
        // Pulo
        if (noChao && (IsKeyPressed(KEY_W) || IsKeyPressed(KEY_SPACE))) {
            velocidadeY = forcaPulo;
            noChao = false;
        }
        // Aplica gravidade
        if (!noChao) {
            posicaoCoracao.y += velocidadeY;
            velocidadeY += gravidade;
            // --- Limite superior pelo osso grande do topo ---
            if (posicaoCoracao.y < limiteSuperior) {
                posicaoCoracao.y = limiteSuperior;
                velocidadeY = 0;
            }
            if (posicaoCoracao.y >= chaoY) {
                posicaoCoracao.y = chaoY;
                velocidadeY = 0;
                noChao = true;
            }
        } else {
            posicaoCoracao.y = chaoY;
        }
    } else {
        if (IsKeyDown(KEY_W) && posicaoCoracao.y > AREA_JOGO_Y + TAMANHO_CORACAO/2) {
            posicaoCoracao.y -= velocidadeCoracao;
            jogadorMovendo = true;
        }
        if (IsKeyDown(KEY_S) && posicaoCoracao.y < AREA_JOGO_Y + AREA_JOGO_ALTURA - TAMANHO_CORACAO/2) {
            posicaoCoracao.y += velocidadeCoracao;
            jogadorMovendo = true;
        }
        if (IsKeyDown(KEY_A) && posicaoCoracao.x > AREA_JOGO_X + TAMANHO_CORACAO/2) {
            posicaoCoracao.x -= velocidadeCoracao;
            jogadorMovendo = true;
        }
        if (IsKeyDown(KEY_D) && posicaoCoracao.x < AREA_JOGO_X + AREA_JOGO_LARGURA - TAMANHO_CORACAO/2) {
            posicaoCoracao.x += velocidadeCoracao;
            jogadorMovendo = true;
        }
    }
}

void gerarObstaculoBranco(void) {
    for (int i = 0; i < 50; i++) {
        if (!obstaculosBrancos[i].ativo) {
            obstaculosBrancos[i].ativo = true;
            obstaculosBrancos[i].posicao.x = AREA_JOGO_X + AREA_JOGO_LARGURA;
            obstaculosBrancos[i].posicao.y = AREA_JOGO_Y + GetRandomValue(0, AREA_JOGO_ALTURA - 30);
            obstaculosBrancos[i].comprimento = GetRandomValue(10, 40);
            obstaculosBrancos[i].velocidade = velocidadeBase * 1.5f + GetRandomValue(0, 200) / 100.0f;
            
            if (GetRandomValue(0, 10) < 3) {
                obstaculosBrancos[i].velocidade *= 1.5f;
                obstaculosBrancos[i].posicao.y = posicaoCoracao.y + GetRandomValue(-50, 50);
            }
            
            break;
        }
    }
}

void gerarObstaculoRoxo(void) {
    if (faseAtual < 2) return;
    
    if (GetRandomValue(0, 100) < 30) {
        for (int i = 0; i < 30; i++) {
            if (!obstaculosRoxos[i].ativo) {
                obstaculosRoxos[i].ativo = true;
                obstaculosRoxos[i].posicao.x = AREA_JOGO_X + AREA_JOGO_LARGURA;
                obstaculosRoxos[i].posicao.y = AREA_JOGO_Y + GetRandomValue(40, AREA_JOGO_ALTURA - 40);
                obstaculosRoxos[i].comprimento = GetRandomValue(2, 4) * 10;
                obstaculosRoxos[i].velocidade = velocidadeBase * 0.8f;
                break;
            }
        }
    }
}

void gerarObstaculoAmarelo(void) {
    if (faseAtual < 3) return;
    
    if (GetRandomValue(0, 100) < 5) {
        for (int i = 0; i < 10; i++) {
            if (!obstaculosAmarelos[i].ativo) {
                obstaculosAmarelos[i].ativo = true;
                obstaculosAmarelos[i].posicao.x = AREA_JOGO_X + AREA_JOGO_LARGURA;
                obstaculosAmarelos[i].posicao.y = AREA_JOGO_Y + GetRandomValue(20, AREA_JOGO_ALTURA - 20);
                obstaculosAmarelos[i].velocidade = velocidadeBase * 1.2f;
                break;
            }
        }
    }
}

void atualizarObstaculosBrancos(void) {
    for (int i = 0; i < 50; i++) {
        if (obstaculosBrancos[i].ativo) {
            obstaculosBrancos[i].posicao.x -= obstaculosBrancos[i].velocidade;
            
            if (faseAtual >= 2) {
                float tempo = GetTime();
                obstaculosBrancos[i].posicao.y += sinf(tempo * 3.0f + i) * 1.5f;
                
                if (obstaculosBrancos[i].posicao.y < AREA_JOGO_Y + 10) {
                    obstaculosBrancos[i].posicao.y = AREA_JOGO_Y + 10;
                }
                if (obstaculosBrancos[i].posicao.y > AREA_JOGO_Y + AREA_JOGO_ALTURA - 10 - 20) {
                    obstaculosBrancos[i].posicao.y = AREA_JOGO_Y + AREA_JOGO_ALTURA - 10 - 20;
                }
            }
            
            if (faseAtual >= 3 && i % 3 == 0) {
                if (obstaculosBrancos[i].posicao.y < posicaoCoracao.y) {
                    obstaculosBrancos[i].posicao.y += 1.0f;
                } else if (obstaculosBrancos[i].posicao.y > posicaoCoracao.y) {
                    obstaculosBrancos[i].posicao.y -= 1.0f;
                }
            }
            
            if (obstaculosBrancos[i].posicao.x < AREA_JOGO_X) {
                obstaculosBrancos[i].ativo = false;
            }
        }
    }
}

void atualizarObstaculosRoxos(void) {
    for (int i = 0; i < 30; i++) {
        if (obstaculosRoxos[i].ativo) {
            obstaculosRoxos[i].posicao.x -= obstaculosRoxos[i].velocidade;
            
            if (obstaculosRoxos[i].posicao.x < AREA_JOGO_X) {
                obstaculosRoxos[i].ativo = false;
            }
        }
    }
}

void atualizarObstaculosAmarelos(void) {
    for (int i = 0; i < MAX_OBSTACULOS_AMARELOS; i++) {
        if (obstaculosAmarelos[i].ativo) {
            obstaculosAmarelos[i].posicao.x -= obstaculosAmarelos[i].velocidade;
            
            float amplitude = 80.0f;
            float frequencia = 0.05f;
            
            obstaculosAmarelos[i].posicao.y = AREA_JOGO_Y + AREA_JOGO_ALTURA/2 + 
                                            amplitude * sinf(obstaculosAmarelos[i].posicao.x * frequencia + 
                                                             obstaculosAmarelos[i].angulo);
            
            obstaculosAmarelos[i].rotacao += 2.0f;
            
            if (obstaculosAmarelos[i].posicao.x < AREA_JOGO_X - 50) {
                obstaculosAmarelos[i].ativo = false;
            }
        }
    }
}

void atualizarObstaculos(void) {
    atualizarObstaculosBrancos();
    atualizarObstaculosRoxos();
    atualizarObstaculosAmarelos();
}

void atualizarFase(void) {
    if (faseAtual == 1 && pontuacao >= 1000.0f) {
        mudarParaFase2();
    }
    
    if (faseAtual == 2 && pontuacao >= 2500.0f) {
        mudarParaFase3();
    }
}

bool detectarColisoes(void) {
    bool colisao = false;
    Rectangle coracaoRect = (Rectangle){posicaoCoracao.x, posicaoCoracao.y, TAMANHO_CORACAO, TAMANHO_CORACAO};
    
    for (int i = 0; i < 50; i++) {
        if (obstaculosBrancos[i].ativo) {
            // Ajuste para ossos verticais na Fase 2: hitbox alinhada ao chão
            float xOsso = obstaculosBrancos[i].posicao.x;
            float yOsso;
            float larguraOsso = 62 * 0.35f; // srcVertical.width * escalaVertical
            float alturaOsso = 153 * 0.35f; // srcVertical.height * escalaVertical
            if (faseAtual == 2) {
                yOsso = AREA_JOGO_Y + AREA_JOGO_ALTURA - alturaOsso / 2;
            } else {
                yOsso = obstaculosBrancos[i].posicao.y;
            }
            if (obstaculosBrancos[i].comprimento < 20) {
                Rectangle hitboxOsso = {
                    xOsso - larguraOsso / 2,
                    yOsso - alturaOsso / 2,
                    larguraOsso,
                    alturaOsso
                };
                Rectangle coracaoRect = {posicaoCoracao.x - 6.5f, posicaoCoracao.y - 7.0f, 13.0f, 14.0f};
                if (CheckCollisionRecs(coracaoRect, hitboxOsso)) {
                    obstaculosBrancos[i].ativo = false;
                    vidaAnterior = vidaCoracao;
                    float danoCausado = 15.0f + faseAtual * 5.0f;
                    vidaCoracao -= danoCausado;
                    ultimoDano = vidaAnterior - vidaCoracao;
                    efeitoDanoTempo = 1.0f;
                    adicionarNumeroDano(danoCausado, posicaoCoracao, true);
                    pontuacao -= 20;
                    if (pontuacao < 0) pontuacao = 0;
                }
            }
        }
    }
    
    for (int i = 0; i < 30; i++) {
        if (obstaculosRoxos[i].ativo) {
            Rectangle obstaculoRect = (Rectangle){
                obstaculosRoxos[i].posicao.x,
                obstaculosRoxos[i].posicao.y,
                10,
                20
            };
            
            if (CheckCollisionRecs(coracaoRect, obstaculoRect)) {
                obstaculosRoxos[i].ativo = false;
                vidaAnterior = vidaCoracao;
                float danoCausado = 25.0f + faseAtual * 7.0f;
                vidaCoracao -= danoCausado;
                ultimoDano = vidaAnterior - vidaCoracao;
                efeitoDanoTempo = 1.5f;
                adicionarNumeroDano(danoCausado, posicaoCoracao, true);
                velocidadeCoracao *= 0.7f;
                pontuacao -= 50;
                if (pontuacao < 0) pontuacao = 0;
                colisao = true;
            }
        }
    }
    
    for (int i = 0; i < 10; i++) {
        if (obstaculosAmarelos[i].ativo) {
            Rectangle obstaculoRect = (Rectangle){
                obstaculosAmarelos[i].posicao.x,
                obstaculosAmarelos[i].posicao.y,
                15,
                15
            };
            
            if (CheckCollisionRecs(coracaoRect, obstaculoRect)) {
                obstaculosAmarelos[i].ativo = false;
                vidaAnterior = vidaCoracao;
                float curaObtida = 10.0f;
                vidaCoracao += curaObtida;
                if (vidaCoracao > 200.0f) vidaCoracao = 200.0f;
                ultimoDano = vidaCoracao - vidaAnterior;
                efeitoDanoTempo = 0.8f;
                adicionarNumeroDano(curaObtida, posicaoCoracao, false);
                pontuacao += 100;
            }
        }
    }
    
    return colisao;
}

void mudarParaFase2(void) {
    velocidadeBase = 3.0f;
    
    for (int i = 0; i < 50; i++) {
        obstaculosBrancos[i].ativo = false;
    }
}

void mudarParaFase3(void) {
    velocidadeBase = 4.0f;
    
    for (int i = 0; i < 50; i++) {
        obstaculosBrancos[i].ativo = false;
    }
    
    for (int i = 0; i < 30; i++) {
        obstaculosRoxos[i].ativo = false;
    }
}

void desenharDebugSpriteSheet(void) {
    // Desenha o sprite sheet no canto superior esquerdo
    DrawTexture(texturaCoracao, 10, 10, WHITE);
    // Novo retângulo de debug alinhado para (129, 12, 12, 11)
    DrawRectangleLines(10 + 129, 10 + 12, 12, 11, RED);
}

void desenharJogo(void) {
    BeginDrawing();
    ClearBackground(BLACK);
    desenharDebugSpriteSheet();
    
    // Desenha o fundo da fase atual
    Color corFundo;
    switch (faseAtual) {
        case 1: corFundo = (Color){20, 20, 30, 255}; break;
        case 2: corFundo = (Color){30, 10, 40, 255}; break;
        case 3: corFundo = (Color){40, 5, 5, 255}; break;
        default: corFundo = (Color){0, 0, 0, 255}; break;
    }
    
    DrawRectangle(AREA_JOGO_X, AREA_JOGO_Y, AREA_JOGO_LARGURA, AREA_JOGO_ALTURA, corFundo);
    
    if (efeitoDanoTempo > 0.0f) {
        float intensidadeEfeito = efeitoDanoTempo;
        if (intensidadeEfeito > 1.0f) intensidadeEfeito = 1.0f;
        
        Color corEfeito;
        if (ultimoDano > 0) {
            corEfeito = (Color){
                255, 
                0, 
                0, 
                (unsigned char)(150 * intensidadeEfeito)
            };
        } else {
            corEfeito = (Color){
                100, 
                255, 
                100, 
                (unsigned char)(120 * intensidadeEfeito)
            };
        }
        
        DrawRectangle(
            AREA_JOGO_X, 
            AREA_JOGO_Y, 
            AREA_JOGO_LARGURA, 
            AREA_JOGO_ALTURA, 
            corEfeito
        );
    }
    
    Color corBorda = WHITE;
    if (modoChefao) {
        float intensidade = 0.5f + 0.5f * sinf(tempoJogo * 5.0f);
        corBorda = (Color){255, (unsigned char)(50 * intensidade), (unsigned char)(50 * intensidade), 255};
    }
    DrawRectangleLines(AREA_JOGO_X, AREA_JOGO_Y, AREA_JOGO_LARGURA, AREA_JOGO_ALTURA, corBorda);
    
    if (efeitosVisuaisAvancados) {
        for (int i = 0; i < 20; i++) {
            float x = AREA_JOGO_X + sinf(tempoJogo * 0.5f + i) * AREA_JOGO_LARGURA/2 + AREA_JOGO_LARGURA/2;
            float y = AREA_JOGO_Y + cosf(tempoJogo * 0.3f + i) * AREA_JOGO_ALTURA/2 + AREA_JOGO_ALTURA/2;
            float tamanho = 1.0f + sinf(tempoJogo * 2.0f + i) * 1.0f;
            
            Color corParticula = (Color){
                (unsigned char)(150 + 50 * sinf(tempoJogo + i)),
                (unsigned char)(100 + 50 * sinf(tempoJogo * 1.3f + i)),
                (unsigned char)(200 + 50 * sinf(tempoJogo * 0.7f + i)),
                (unsigned char)(50 + 30 * sinf(tempoJogo * 2.0f + i))
            };
            
            DrawCircleV((Vector2){x, y}, tamanho, corParticula);
        }
    }
    
    desenharObstaculos();
    // --- Corrigido: Desenhar osso grande do topo centralizado e alinhado ---
    if (faseAtual == 2) {
        Rectangle srcOssoTopo = {278, 572, 12, 202};
        // Escala para que o osso cubra ~1/6 da largura do campo, mantendo proporção
        float escalaOssoTopo = (float)AREA_JOGO_LARGURA / (12.0f * 6.0f);
        float larguraOsso = 12 * escalaOssoTopo;
        float alturaOsso = 202 * escalaOssoTopo;
        Rectangle dstOssoTopo = {
            AREA_JOGO_X + AREA_JOGO_LARGURA / 2.0f - larguraOsso / 2.0f, // X: centralizado
            AREA_JOGO_Y - 2.0f, // Y: topo exato da área de jogo
            larguraOsso,
            alturaOsso
        };
        // Origem: topo central do sprite
        Vector2 origem = {larguraOsso / 2.0f, 0};
        DrawTexturePro(
            texturaOssoGrandeTopo,
            srcOssoTopo,
            dstOssoTopo,
            origem,
            0,
            WHITE
        );
    }
    
    atualizarBosses();
    atualizarProjeteis();
    desenharProjeteis();
    desenharBosses();

    desenharCoracao();
    
    desenharNumerosDano();
    
    int barraHPLargura = 200;
    int barraHPAltura = 30;
    int barraHPX = AREA_JOGO_X + (AREA_JOGO_LARGURA - barraHPLargura) / 2;
    int barraHPY = AREA_JOGO_Y + AREA_JOGO_ALTURA + 15;
    
    DrawRectangleGradientH(
        barraHPX, 
        barraHPY, 
        barraHPLargura, 
        barraHPAltura, 
        (Color){60, 0, 0, 220}, 
        (Color){120, 0, 0, 220}
    );
    
    float pulsacaoBarra = 1.0f + 0.05f * sinf(tempoJogo * 4.0f);
    Color corBarraHP = (Color){220, 0, 0, 220};
    
    if (vidaCoracao < 50) {
        corBarraHP.r = 220 + (unsigned char)(35 * sinf(tempoJogo * 10.0f));
        corBarraHP.g = 0;
        pulsacaoBarra = 1.0f + 0.1f * sinf(tempoJogo * 8.0f);
    } else if (vidaCoracao < 100) {
        corBarraHP.r = 220;
        corBarraHP.g = 100;
    }
    
    DrawRectangle(
        barraHPX, 
        barraHPY, 
        (int)(barraHPLargura * (vidaCoracao / 200.0f) * pulsacaoBarra), 
        barraHPAltura, 
        corBarraHP
    );
    
    Color corBordaBarra = WHITE;
    if (vidaCoracao < 50) {
        corBordaBarra.a = 128 + (unsigned char)(127 * sinf(tempoJogo * 8.0f));
    }
    DrawRectangleLines(barraHPX, barraHPY, barraHPLargura, barraHPAltura, corBordaBarra);
    
    char textoHP[20];
    sprintf(textoHP, "HP: %d/200", (int)vidaCoracao);
    int textoHPLargura = MeasureText(textoHP, 20);
    
    DrawText(textoHP, barraHPX + (barraHPLargura - textoHPLargura) / 2 + 1, barraHPY + 6 + 1, 20, BLACK);
    DrawText(textoHP, barraHPX + (barraHPLargura - textoHPLargura) / 2, barraHPY + 6, 20, WHITE);
    
    {
        char textoFase[50];
        
        switch(faseAtual) {
            case 1: sprintf(textoFase, "FASE 1: LEMBRANÇAS"); break;
            case 2: sprintf(textoFase, "FASE 2: PESADELOS"); break;
            case 3: sprintf(textoFase, "FASE 3: CONFRONTO FINAL"); break;
            default: sprintf(textoFase, "FASE %d", faseAtual); break;
        }
        
        DrawRectangle(AREA_JOGO_X, 8, MeasureText(textoFase, 20) + 10, 24, (Color){0, 0, 0, 180});
        DrawText(textoFase, AREA_JOGO_X + 5, 10, 20, WHITE);
    }
    
    char textoBoss[50] = "";
    char textoPontuacao[50];
    
    if (faseAtual >= 1 && faseAtual <= 3) {
        switch (faseAtual) {
            case 1: sprintf(textoBoss, "Boss: Memória Dolorosa"); break;
            case 2: sprintf(textoBoss, "Boss: Trauma Profundo"); break;
            case 3: sprintf(textoBoss, "Boss: Emoção Caótica"); break;
            default: textoBoss[0] = '\0';
        }
    }
    
    sprintf(textoPontuacao, "Pontuação: %d", (int)pontuacao);
    
    if (textoBoss[0] != '\0') {
        int larguraBoss = MeasureText(textoBoss, 20) + 20;
        DrawRectangle(
            AREA_JOGO_X + AREA_JOGO_LARGURA - larguraBoss - 10,
            AREA_JOGO_Y + 10,
            larguraBoss,
            30,
            (Color){ 50, 0, 0, 180 }
        );
        
        DrawText(
            textoBoss, 
            AREA_JOGO_X + AREA_JOGO_LARGURA - larguraBoss - 0,
            AREA_JOGO_Y + 15, 
            20, 
            (Color){255, 100, 100, 255}
        );
    }
    
    int larguraPontuacao = MeasureText(textoPontuacao, 20) + 20;
    DrawRectangle(
        AREA_JOGO_X + 10,
        AREA_JOGO_Y + 10,
        larguraPontuacao,
        30,
        (Color){ 30, 30, 0, 180 }
    );
    
    DrawText(
        textoPontuacao, 
        AREA_JOGO_X + 20, 
        AREA_JOGO_Y + 15, 
        20, 
        (Color){255, 255, 0, 255}
    );
}

void desenharCoracao(void) {
    static float tempo = 0.0f;
    tempo += GetFrameTime();
    
    float escala = 1.0f + 0.05f * sinf(tempo * 2.5f);
    float rotacao = 2.0f * sinf(tempo * 1.5f);
    Vector2 centro = { 6.0f, 5.5f }; // centro ajustado para 12x11
    float offsetX = 0.0f;
    float offsetY = 0.0f;
    if (vidaCoracao < 50.0f) {
        offsetX = GetRandomValue(-1, 1);
        offsetY = GetRandomValue(-1, 1);
        escala = 1.0f + 0.08f * sinf(tempo * 6.0f);
    }
    Color corBase = WHITE;
    // Recorte final alinhado
    Rectangle src = {124, 6, 10, 11};
    Rectangle dst = {
        posicaoCoracao.x + offsetX,
        posicaoCoracao.y + offsetY,
        13 * escala,
        14 * escala
    };
    DrawTexturePro(
        texturaCoracao,
        src,
        dst,
        centro,
        rotacao,
        corBase
    );
    // ...rastro e efeitos, se necessário...
}

void desenharObstaculos(void) {
    // Região correta da spritesheet ossos.png
    Rectangle srcVertical = {53, 36, 62, 153}; // Osso vertical (em pé)
    float escalaVertical = 0.35f;  // Escala fiel ao Undertale
    for (int i = 0; i < MAX_OBSTACULOS_BRANCOS; i++) {
        if (obstaculosBrancos[i].ativo) {
            float xOsso = obstaculosBrancos[i].posicao.x;
            float yOsso;
            if (faseAtual == 2) {
                // Alinha a base do osso ao chão
                yOsso = AREA_JOGO_Y + AREA_JOGO_ALTURA - (srcVertical.height * escalaVertical) / 2;
            } else {
                yOsso = obstaculosBrancos[i].posicao.y;
            }
            if (obstaculosBrancos[i].comprimento < 20) {
                DrawTexturePro(
                    texturaOssoReto,
                    srcVertical,
                    (Rectangle){xOsso - (srcVertical.width * escalaVertical) / 2, yOsso - (srcVertical.height * escalaVertical) / 2, srcVertical.width * escalaVertical, srcVertical.height * escalaVertical},
                    (Vector2){0,0},
                    0,
                    WHITE
                );
            }
        }
    }
    for (int i = 0; i < MAX_OBSTACULOS_ROXOS; i++) {
        if (obstaculosRoxos[i].ativo) {
            float xProj = obstaculosRoxos[i].posicao.x;
            float yProj = obstaculosRoxos[i].posicao.y;
            float raioProj = 12.0f;
            Color corPrincipal = (Color){180, 50, 220, 255};
            DrawCircle(xProj, yProj, raioProj, corPrincipal);
        }
    }
    for (int i = 0; i < MAX_OBSTACULOS_AMARELOS; i++) {
        if (obstaculosAmarelos[i].ativo) {
            float tempo = GetTime();
            float xMoeda = obstaculosAmarelos[i].posicao.x;
            float yMoeda = obstaculosAmarelos[i].posicao.y;
            float raioMoeda = 10.0f;
            float angulo = tempo * 120.0f;
            
            float escala = 1.0f + 0.1f * sinf(tempo * 5.0f);
            raioMoeda *= escala;
            
            Color corOuro = (Color){255, 215, 0, 255};
            Color corBrilho = (Color){255, 255, 200, 255};
            
            DrawCircle(xMoeda, yMoeda, raioMoeda, corOuro);
            
            DrawCircle(xMoeda, yMoeda, raioMoeda * 0.8f, corBrilho);
            
            DrawText("$", xMoeda - 5, yMoeda - 10, 20, (Color){220, 160, 0, 255});
            
            for (int j = 0; j < 4; j++) {
                float brilhoAngulo = angulo + j * 90.0f;
                float xBrilho = xMoeda + (raioMoeda * 0.9f) * cosf(brilhoAngulo * DEG2RAD);
                float yBrilho = yMoeda + (raioMoeda * 0.9f) * sinf(brilhoAngulo * DEG2RAD);
                
                DrawCircle(xBrilho, yBrilho, 2.0f, WHITE);
            }
            
            for (int j = 0; j < 2; j++) {
                float raioAura = raioMoeda * (1.2f + 0.2f * j) * (1.0f + 0.1f * sinf(tempo * 8.0f + j));
                DrawCircleLines(xMoeda, yMoeda, raioAura, (Color){255, 215, 0, 100 - j * 40});
            }
            
            if (GetRandomValue(0, 10) < 3) {
                float angParticula = GetRandomValue(0, 360) * DEG2RAD;
                float distParticula = raioMoeda * 1.2f;
                float xParticula = xMoeda + distParticula * cosf(angParticula);
                float yParticula = yMoeda + distParticula * sinf(angParticula);
                
                DrawCircle(xParticula, yParticula, 1.5f, (Color){255, 255, 200, 200});
            }
        }
    }
}

void inicializarNumerosDano(void) {
    for (int i = 0; i < MAX_NUMEROS_DANO; i++) {
        numerosDano[i].ativo = false;
    }
}

void adicionarNumeroDano(float valor, Vector2 posicao, bool ehDano) {
    for (int i = 0; i < MAX_NUMEROS_DANO; i++) {
        if (!numerosDano[i].ativo) {
            numerosDano[i].posicao = posicao;
            numerosDano[i].valor = valor;
            numerosDano[i].tempo = 1.0f;
            numerosDano[i].ativo = true;
            numerosDano[i].ehDano = ehDano;
            
            numerosDano[i].velocidade.x = GetRandomValue(-50, 50) / 100.0f;
            numerosDano[i].velocidade.y = -2.5f;
            
            if (ehDano) {
                numerosDano[i].cor = (Color){255, 0, 0, 255};
            } else {
                numerosDano[i].cor = (Color){0, 255, 0, 255};
            }
            
            break;
        }
    }
}

void atualizarNumerosDano(void) {
    for (int i = 0; i < MAX_NUMEROS_DANO; i++) {
        if (numerosDano[i].ativo) {
            numerosDano[i].posicao.x += numerosDano[i].velocidade.x;
            numerosDano[i].posicao.y += numerosDano[i].velocidade.y;
            
            numerosDano[i].velocidade.y *= 0.95f;
            
            numerosDano[i].tempo -= GetFrameTime();
            
            if (numerosDano[i].tempo <= 0) {
                numerosDano[i].ativo = false;
            }
        }
    }
}

void desenharNumerosDano(void) {
    for (int i = 0; i < MAX_NUMEROS_DANO; i++) {
        if (numerosDano[i].ativo) {
            int alpha = (int)(255 * numerosDano[i].tempo);
            if (alpha < 0) alpha = 0;
            if (alpha > 255) alpha = 255;
            
            Color color = numerosDano[i].cor;
            color.a = alpha;
            
            float size = 20.0f;
            if (numerosDano[i].valor > 30.0f) size = 30.0f; 
            if (numerosDano[i].valor > 50.0f) size = 35.0f;
            
            float pulseFactor = 1.0f + 0.2f * sinf(GetTime() * 10.0f);
            size *= pulseFactor;
            
            char texto[20];
            if (numerosDano[i].ehDano) {
                sprintf(texto, "-%d", (int)numerosDano[i].valor);
            } else {
                sprintf(texto, "+%d", (int)numerosDano[i].valor);
            }
            
            DrawText(texto, numerosDano[i].posicao.x + 2, numerosDano[i].posicao.y + 2, size, BLACK);
            DrawText(texto, numerosDano[i].posicao.x, numerosDano[i].posicao.y, size, color);
        }
    }
}

void desenharProjeteis(void) {
    for (int i = 0; i < MAX_BOSSES; i++) {
        for (int j = 0; j < MAX_PROJETEIS; j++) {
            if (!bosses[i].projeteis[j].ativo) continue;
            if (bosses[i].projeteis[j].usaSprite) {
                DrawTexturePro( bosses[i].projeteis[j].textura,
                                (Rectangle){0,0, bosses[i].projeteis[j].textura.width, bosses[i].projeteis[j].textura.height},
                                (Rectangle){ bosses[i].projeteis[j].posicao.x, bosses[i].projeteis[j].posicao.y,
                                             bosses[i].projeteis[j].textura.width * bosses[i].projeteis[j].escala,
                                             bosses[i].projeteis[j].textura.height * bosses[i].projeteis[j].escala },
                                (Vector2){ bosses[i].projeteis[j].textura.width/2, bosses[i].projeteis[j].textura.height/2 },
                                bosses[i].projeteis[j].rotacao, WHITE );
            } else {
                DrawCircleV(bosses[i].projeteis[j].posicao,
                            bosses[i].projeteis[j].raio * bosses[i].projeteis[j].escala,
                            bosses[i].projeteis[j].cor );
            }
        }
    }
}

void atualizarProjeteis(void) {
    for (int i = 0; i < MAX_BOSSES; i++) {
        for (int j = 0; j < MAX_PROJETEIS; j++) {
            if (!bosses[i].projeteis[j].ativo) continue;
            bosses[i].projeteis[j].posicao.x += bosses[i].projeteis[j].velocidade.x;
            bosses[i].projeteis[j].posicao.y += bosses[i].projeteis[j].velocidade.y;
        }
    }
}

void ativarBossDaFase(int fase) {
    for (int i = 0; i < MAX_BOSSES; i++) {
        bosses[i].ativo = (bosses[i].fase == fase);
    }
}

enum Direcao { ESQUERDA = -1, DIREITA = 1 };
static int direcaoBoss = DIREITA;
static float velocidadeBoss = 150.0f;

void atualizarBosses(void) {
    for (int i = 0; i < MAX_BOSSES; i++) {
        if (!bosses[i].ativo) continue;

        if (bosses[i].vida <= 0) {
            bosses[i].ativo = false;

            faseAtual++;
            if (faseAtual > MAX_BOSSES) faseAtual = MAX_BOSSES;

            ativarBossDaFase(faseAtual);
            continue;
        }

        bosses[i].posicao.x += direcaoBoss * velocidadeBoss * GetFrameTime();

        float halfW = bosses[i].frameAtual.width * 0.5f * 0.5f;
        float minX = AREA_JOGO_X + halfW;
        float maxX = AREA_JOGO_X + AREA_JOGO_LARGURA - halfW;

        if (bosses[i].posicao.x <= minX) {
            bosses[i].posicao.x = minX;
            direcaoBoss = DIREITA;
        } else if (bosses[i].posicao.x >= maxX) {
            bosses[i].posicao.x = maxX;
            direcaoBoss = ESQUERDA;
        }
    }
}

extern Boss bosses[MAX_BOSSES];
