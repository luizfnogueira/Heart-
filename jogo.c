#include "jogo.h"
#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include "raylib.h"
#include <string.h>
#include "obstaculo_sprites.h"
#include "raymath.h"


void adicionarNumeroDano(float valor, Vector2 posicao, bool ehDano);

// Variáveis globais do jogo
Vector2 posicaoCoracao;
float vidaCoracao = 200.0f;
float pontuacao = 0.0f;
int faseAtual = 1;
NivelDificuldade dificuldadeAtual = NORMAL;
Texture2D texturaCoracao;
Texture2D texturaOssoReto;
Texture2D texturaOssoHorizontal;
float tempoInvulnerabilidade = 0.0f;


// Corrigido: variáveis de tempo de onda/obstáculo
static float tempoUltimaOnda = 0.0f;
static float tempoUltimoObstaculo = 0.0f;

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

// Array de números de dano flutuantes
static NumeroDano numerosDano[MAX_NUMEROS_DANO];

// Arrays de obstáculos
static Obstaculo obstaculosBrancos[MAX_OBSTACULOS_BRANCOS];    

static bool efeitosVisuaisAvancados = true;

// --- Parâmetros para variação de tamanho da tela ---
static float tempoTela = 0.0f;
static float escalaTela = 1.0f;

// Chame isso no início de desenharJogo()
void atualizarEscalaTela(void) {
    tempoTela += GetFrameTime();
    // Variação pulsante e caótica
    float base = 1.0f + 0.08f * sinf(tempoTela * 2.5f);
    float caos = 0.04f * sinf(tempoTela * 7.0f + sinf(tempoTela * 2.0f));
    escalaTela = base + caos;
}

// Use esta função para desenhar tudo dentro da área de jogo com escala
void BeginAreaJogoComEscala(void) {
    Vector2 centro = {AREA_JOGO_X + AREA_JOGO_LARGURA/2, AREA_JOGO_Y + AREA_JOGO_ALTURA/2};
    BeginMode2D((Camera2D){
        .offset = centro,
        .target = centro,
        .rotation = 0,
        .zoom = escalaTela
    });
}
void EndAreaJogoComEscala(void) {
    EndMode2D();
}

// Limpa avisos de laser
void limparAvisosLaser(void) {
}

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
      // Inicializa avisos de laser
    limparAvisosLaser();
    
    // Inicializa o sistema de charadas
    inicializarSistemaCharadas();
    modoCharada = false;
}

// --- Liberar textura do osso grande do topo ao fechar o jogo ---
void liberarRecursosJogo(void) {
    UnloadTexture(texturaCoracao);
    UnloadTexture(texturaOssoReto);
    UnloadTexture(texturaOssoHorizontal);
    
    // Liberar recursos do sistema de charadas
    liberarRecursosCharada();
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
        obstaculosBrancos[i].tempoAnimacao = 0;
    }
}

// Gera ossos brancos de forma dinâmica e aleatória na Fase 1
void gerarObstaculoBrancoAleatorio(void) {
    float escalaVertical = 0.35f;
    for (int i = 0; i < MAX_OBSTACULOS_BRANCOS; i++) {
        if (!obstaculosBrancos[i].ativo) {
            obstaculosBrancos[i].ativo = true;
            // Limitamos a posição x para estar EXATAMENTE no limite direito da área visível (sem ultrapassar)
            obstaculosBrancos[i].posicao.x = AREA_JOGO_X + AREA_JOGO_LARGURA;

            bool vertical = (GetRandomValue(0, 1) == 1);

            float alturaOsso;
            float minY, maxY;

            if (vertical) {
                obstaculosBrancos[i].comprimento = 10; // vertical
                alturaOsso = 153 * escalaVertical;
            } else {
                obstaculosBrancos[i].comprimento = 30; // horizontal
                alturaOsso = 13 * escalaVertical;
            }

            minY = AREA_JOGO_Y + alturaOsso / 2 + 5;
            maxY = AREA_JOGO_Y + AREA_JOGO_ALTURA - alturaOsso / 2 - 5;
            int sorteioY = GetRandomValue((int)minY, (int)maxY);
            obstaculosBrancos[i].posicao.y = Clamp((float)sorteioY, minY, maxY);

            obstaculosBrancos[i].velocidade = velocidadeBase * (1.5f + GetRandomValue(0, 10) / 10.0f);
            
            // Garantir que velocidade seja negativa (para esquerda) para que o obstáculo entre no campo de visão
            if (obstaculosBrancos[i].velocidade > 0) {
                obstaculosBrancos[i].velocidade *= -1;
            }
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
    
    // Determinar a altura máxima para garantir que todos os ossos fiquem dentro da área visível
    float alturaAreaVisivel = AREA_JOGO_ALTURA - 40; // 20px de margem em cima e embaixo
    float alturaMaximaVertical = 153 * escalaVertical;
    float alturaMaximaHorizontal = 13 * escalaVertical;
    float alturaMaximaTotal = 0;
    
    // Calcular a altura total necessária para todos os ossos com espaçamento
    for (int i = 0; i < quantidade; i++) {
        if (i % 2 == 0) {
            alturaMaximaTotal += alturaMaximaHorizontal + 10;
        } else {
            alturaMaximaTotal += alturaMaximaVertical + 10;
        }
    }
    
    // Ajustar a quantidade se necessário para caber na área visível
    if (alturaMaximaTotal > alturaAreaVisivel) {
        quantidade = (int)(quantidade * (alturaAreaVisivel / alturaMaximaTotal));
        if (quantidade < 3) quantidade = 3; // Garantir pelo menos alguns ossos
    }
    
    for (int i = 0; i < quantidade; i++) {
        for (int j = 0; j < MAX_OBSTACULOS_BRANCOS; j++) {
            if (!obstaculosBrancos[j].ativo) {
                obstaculosBrancos[j].ativo = true;
                // Posicionar exatamente no limite direito da área de jogo
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
                
                // Verificar se a posição Y está dentro dos limites da área de jogo
                float yPosicao = yAtual + alturaOsso / 2;
                if (yPosicao > AREA_JOGO_Y + AREA_JOGO_ALTURA - alturaOsso/2) {
                    yPosicao = AREA_JOGO_Y + AREA_JOGO_ALTURA - alturaOsso/2;
                }
                
                obstaculosBrancos[j].posicao.y = yPosicao;
                yAtual += espacamento;
                
                // Velocidade negativa para mover para a esquerda
                obstaculosBrancos[j].velocidade = -velocidadeBase * 1.7f;
                break;
            }
        }
    }
}

// --- Nova função: Sans Fight para todas as fases ---
static float sansTempoAtaque = 0.0f;
static int sansPadraoAtual = 0;
static float sansIntervaloPadrao = 2.0f;

void sansFightFase(void) {
    // Box de esquiva (área de movimento do coração)
    DrawRectangleLines(AREA_JOGO_X, AREA_JOGO_Y, AREA_JOGO_LARGURA, AREA_JOGO_ALTURA, (Color){255,255,255,180});
    
    // Margem de segurança para obstáculos (pixels além da borda da área de jogo)
    float margemSeguranca = 50.0f;
    
    // Atualiza obstáculos (ossos) e remove obstáculos fora da área de jogo
    for (int i = 0; i < MAX_OBSTACULOS_BRANCOS; i++) {
        if (obstaculosBrancos[i].ativo) {
            float escalaVertical = 0.35f;
            float larguraOsso = (obstaculosBrancos[i].comprimento == 10) ? 
                               62 * escalaVertical : 62 * escalaVertical;
            float alturaOsso = (obstaculosBrancos[i].comprimento == 10) ? 
                               153 * escalaVertical : 13 * escalaVertical;
            
            // Removes any obstacle that is completely outside the game area with a safety margin
            if (obstaculosBrancos[i].posicao.x - larguraOsso/2 > AREA_JOGO_X + AREA_JOGO_LARGURA + margemSeguranca ||
                obstaculosBrancos[i].posicao.x + larguraOsso/2 < AREA_JOGO_X - margemSeguranca ||
                obstaculosBrancos[i].posicao.y - alturaOsso/2 > AREA_JOGO_Y + AREA_JOGO_ALTURA + margemSeguranca ||
                obstaculosBrancos[i].posicao.y + alturaOsso/2 < AREA_JOGO_Y - margemSeguranca) {
                
                // Se o obstáculo estiver completamente fora da área, desativamos
                obstaculosBrancos[i].ativo = false;
                continue;
            }
              // Atualiza a posição com base na sua velocidade e tipo
            if (obstaculosBrancos[i].comprimento == 10) {
                // Ossos verticais se movem na direção da velocidade (normalmente para cima)
                obstaculosBrancos[i].posicao.y += obstaculosBrancos[i].velocidade;
            } else {
                // Ossos horizontais se movem na direção da velocidade (normalmente para a esquerda)
                obstaculosBrancos[i].posicao.x += obstaculosBrancos[i].velocidade;
            }
        }
    }
    
    // Ataques de ossos horizontais e verticais, padrões variam conforme a fase
    sansTempoAtaque += GetFrameTime();
    float intervaloBase = 1.2f - 0.2f * (faseAtual-1); // Fases mais avançadas = ataques mais rápidos
    if (intervaloBase < 0.5f) intervaloBase = 0.5f;
    
    if (sansTempoAtaque > sansIntervaloPadrao * intervaloBase) {
        sansTempoAtaque = 0;
        // Mais padrões nas fases avançadas
        int maxPadrao = (faseAtual >= 3) ? 3 : 2;
        sansPadraoAtual = GetRandomValue(0, maxPadrao); // 0: horizontal, 1: vertical, 2: mista, 3: caveiras
        sansIntervaloPadrao = 1.0f + GetRandomValue(0, 10) / 10.0f;
          if (sansPadraoAtual == 0) {
            // Ossos horizontais subindo do chão
            for (int i = 0; i < 6 + faseAtual; i++) {
                for (int j = 0; j < MAX_OBSTACULOS_BRANCOS; j++) {
                    if (!obstaculosBrancos[j].ativo) {
                        obstaculosBrancos[j].ativo = true;
                        // Garantir que as posições X estejam distribuídas dentro da área de jogo
                        float minX = AREA_JOGO_X + 30;
                        float maxX = AREA_JOGO_X + AREA_JOGO_LARGURA - 30;
                        float spacing = (maxX - minX) / (6 + faseAtual);
                        obstaculosBrancos[j].posicao.x = minX + i * spacing;
                        // Posicionar exatamente no limite inferior da área de jogo
                        obstaculosBrancos[j].posicao.y = AREA_JOGO_Y + AREA_JOGO_ALTURA;
                        // Velocidade negativa para mover para cima
                        obstaculosBrancos[j].velocidade = -4.0f - GetRandomValue(0, 2) - faseAtual;
                        obstaculosBrancos[j].comprimento = 10; // vertical
                        break;
                    }
                }
            }
        } else if (sansPadraoAtual == 1) {            // Ossos verticais vindo da direita
            for (int i = 0; i < 5 + faseAtual; i++) {
                for (int j = 0; j < MAX_OBSTACULOS_BRANCOS; j++) {
                    if (!obstaculosBrancos[j].ativo) {
                        obstaculosBrancos[j].ativo = true;
                        // Posicionar exatamente no limite direito da área visível
                        obstaculosBrancos[j].posicao.x = AREA_JOGO_X + AREA_JOGO_LARGURA;
                        // Garantir que o eixo Y esteja dentro dos limites visíveis
                        float minY = AREA_JOGO_Y + 20;
                        float maxY = AREA_JOGO_Y + AREA_JOGO_ALTURA - 20;
                        float yPos = AREA_JOGO_Y + 30 + i * 30;
                        obstaculosBrancos[j].posicao.y = Clamp(yPos, minY, maxY);
                        // Velocidade negativa para mover para a esquerda
                        obstaculosBrancos[j].velocidade = -5.0f - GetRandomValue(0, 2) - faseAtual;
                        obstaculosBrancos[j].comprimento = 30; // horizontal
                        break;
                    }
                }
            }
        } else if (sansPadraoAtual == 2) {            // Misto: ossos de baixo e da direita
            for (int i = 0; i < 3 + faseAtual; i++) {
                for (int j = 0; j < MAX_OBSTACULOS_BRANCOS; j++) {
                    if (!obstaculosBrancos[j].ativo) {
                        obstaculosBrancos[j].ativo = true;
                        // Garantir que as posições X estejam dentro da área de jogo
                        float minX = AREA_JOGO_X + 30;
                        float maxX = AREA_JOGO_X + AREA_JOGO_LARGURA - 30;
                        float xPos = AREA_JOGO_X + 30 + i * 60;
                        obstaculosBrancos[j].posicao.x = Clamp(xPos, minX, maxX);
                         // Posicionar no limite inferior da área de jogo
                        obstaculosBrancos[j].posicao.y = AREA_JOGO_Y + AREA_JOGO_ALTURA;
                        obstaculosBrancos[j].velocidade = -6.0f - faseAtual;
                        obstaculosBrancos[j].comprimento = 10;
                        break;
                    }
                }
            }
            for (int i = 0; i < 2 + faseAtual; i++) {
                for (int j = 0; j < MAX_OBSTACULOS_BRANCOS; j++) {
                    if (!obstaculosBrancos[j].ativo) {
                        obstaculosBrancos[j].ativo = true;
                        // Posicionar exatamente no limite direito da área de jogo
                        obstaculosBrancos[j].posicao.x = AREA_JOGO_X + AREA_JOGO_LARGURA;
                        // Garantir que as posições Y estejam dentro da área de jogo
                        float minY = AREA_JOGO_Y + 30;
                        float maxY = AREA_JOGO_Y + AREA_JOGO_ALTURA - 30;
                        float yPos = AREA_JOGO_Y + 60 + i * 60;
                        obstaculosBrancos[j].posicao.y = Clamp(yPos, minY, maxY);
                        obstaculosBrancos[j].velocidade = -7.0f - faseAtual;
                        obstaculosBrancos[j].comprimento = 30;
                        break;
                    }
                }
            }
        } else if (sansPadraoAtual == 3) {
        }
    }
    // Atualiza obstáculos (ossos)
    for (int i = 0; i < MAX_OBSTACULOS_BRANCOS; i++) {
        if (obstaculosBrancos[i].ativo) {
            if (obstaculosBrancos[i].comprimento == 10) {
                // Ossos verticais sobem
                obstaculosBrancos[i].posicao.y += obstaculosBrancos[i].velocidade;
                if (obstaculosBrancos[i].posicao.y < AREA_JOGO_Y - 40) 
                    obstaculosBrancos[i].ativo = false;
            } else {
                // Ossos horizontais vêm da direita
                obstaculosBrancos[i].posicao.x += obstaculosBrancos[i].velocidade;
                if (obstaculosBrancos[i].posicao.x < AREA_JOGO_X - 40) 
                    obstaculosBrancos[i].ativo = false;
            }
        }
    }
}

// Limpa todos os obstáculos e prepara o jogo para a próxima fase
void limparObstaculosEPrepararProximaFase(void) {
    // Limpa todos os obstáculos brancos
    for (int i = 0; i < MAX_OBSTACULOS_BRANCOS; i++) {
        obstaculosBrancos[i].ativo = false;
    }
    
    // Limpa avisos de laser
    limparAvisosLaser();
}


bool atualizarJogo(void) {
    // Verificar se estamos no modo charada
    if (modoCharada) {
        return atualizarTelaCharada(); // Retorna false se o jogador falhar na charada
    }
    
    // Verifica transições de fase
    if (pontuacao >= 200 && faseAtual == 1) {
        // Ativar charada para passar para a fase 2
        gerarNovaCharada(2);
        modoCharada = true;
        return true;
    }

    if (pontuacao >= 400 && faseAtual == 2) {
        // Ativar charada para passar para a fase 3
        gerarNovaCharada(3);
        modoCharada = true;
        return true;
    }

    if (pontuacao >= 2000 && faseAtual == 3) {
        // Charada final (opcional)
        gerarNovaCharada(4); // Charada de vitória
        modoCharada = true;
        return true;
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
        float intervalo = 1.0f + (GetRandomValue(0, 10) / 15.0f); // 1s a 1.67s
        if (tempoAtual - tempoUltimoObstaculo > intervalo) {
            gerarObstaculoBrancoAleatorio();
            tempoUltimoObstaculo = tempoAtual;
        }
    } else if (faseAtual == 2) {
        // Fase 2: caveiras animadas que disparam lasers
        float intervalo = 1.5f + (GetRandomValue(0, 10) / 10.0f); // 1.5s a 2.5s
        if (tempoAtual - tempoUltimoObstaculo > intervalo) {
            tempoUltimoObstaculo = tempoAtual;
        }
    } else if (tempoAtual - tempoUltimaOnda > 2.0f) {
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

    // Atualiza tempo de invulnerabilidade
    if (tempoInvulnerabilidade > 0.0f) {
        tempoInvulnerabilidade -= GetFrameTime();
        if (tempoInvulnerabilidade < 0.0f)
            tempoInvulnerabilidade = 0.0f;
    }

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
    // Movimento livre em todas as fases (WASD)
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

void gerarObstaculoBranco(void) {
    for (int i = 0; i < 50; i++) {
        if (!obstaculosBrancos[i].ativo) {
            obstaculosBrancos[i].ativo = true;
            // Posicionar exatamente no limite direito da área de jogo
            obstaculosBrancos[i].posicao.x = AREA_JOGO_X + AREA_JOGO_LARGURA;
            
            // Garantir que a posição Y esteja dentro dos limites da área de jogo
            float minY = AREA_JOGO_Y + 10;
            float maxY = AREA_JOGO_Y + AREA_JOGO_ALTURA - 30;
            float randomY = AREA_JOGO_Y + GetRandomValue(10, AREA_JOGO_ALTURA - 40);
            obstaculosBrancos[i].posicao.y = Clamp(randomY, minY, maxY);
            
            // Definir comprimento adequado (10 para vertical, 30 para horizontal)
            obstaculosBrancos[i].comprimento = (GetRandomValue(0, 1) == 0) ? 10 : 30;
            
            // Garantir que a velocidade seja sempre negativa (para esquerda)
            obstaculosBrancos[i].velocidade = -velocidadeBase * 1.5f - GetRandomValue(0, 200) / 100.0f;
            
            // Adicionar variação para obstáculos que miram no jogador
            if (GetRandomValue(0, 10) < 3) {
                obstaculosBrancos[i].velocidade *= 1.5f;
                // Ajustar posição Y para se aproximar do jogador, mas ainda dentro dos limites da área
                float targetY = posicaoCoracao.y + GetRandomValue(-50, 50);
                obstaculosBrancos[i].posicao.y = Clamp(targetY, minY, maxY);
            }
            
            break;
        }
    }
}

// --- Adição ao final de atualizarObstaculosBrancos ---
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
                if (obstaculosBrancos[i].posicao.y > AREA_JOGO_Y + AREA_JOGO_ALTURA - 30) {
                    obstaculosBrancos[i].posicao.y = AREA_JOGO_Y + AREA_JOGO_ALTURA - 30;
                }
            }
            if (faseAtual >= 3 && i % 3 == 0) {
                if (obstaculosBrancos[i].posicao.y < posicaoCoracao.y) {
                    obstaculosBrancos[i].posicao.y += 1.0f;
                } else if (obstaculosBrancos[i].posicao.y > posicaoCoracao.y) {
                    obstaculosBrancos[i].posicao.y -= 1.0f;
                }
            }

            if (obstaculosBrancos[i].posicao.x < AREA_JOGO_X - 60 ||
                obstaculosBrancos[i].posicao.x > AREA_JOGO_X + AREA_JOGO_LARGURA + 60 ||
                obstaculosBrancos[i].posicao.y < AREA_JOGO_Y - 60 ||
                obstaculosBrancos[i].posicao.y > AREA_JOGO_Y + AREA_JOGO_ALTURA + 60) {
                obstaculosBrancos[i].ativo = false;
            }
        }
    }
}


void atualizarObstaculos(void) {
    float escalaVertical = 0.35f;
    float larguraVertical = 62 * escalaVertical;    // Largura do osso vertical
    float larguraHorizontal = 62 * escalaVertical;  // Largura do osso horizontal
    
    for (int i = 0; i < MAX_OBSTACULOS_BRANCOS; i++) {
        if (obstaculosBrancos[i].ativo) {
            // Atualiza a posição do obstáculo com base na velocidade
            obstaculosBrancos[i].posicao.x += obstaculosBrancos[i].velocidade;
            
            // Calcular a largura real do obstáculo com base no tipo
            float larguraReal = (obstaculosBrancos[i].comprimento == 10) ? 
                               larguraVertical : larguraHorizontal;
                               
            // Verificar se o obstáculo está completamente fora da área visível
            // Usamos a largura real para garantir que não haja obstáculos invisíveis
            if (obstaculosBrancos[i].posicao.x + larguraReal/2 < AREA_JOGO_X || 
                obstaculosBrancos[i].posicao.x - larguraReal/2 > AREA_JOGO_X + AREA_JOGO_LARGURA ||
                obstaculosBrancos[i].posicao.y > AREA_JOGO_Y + AREA_JOGO_ALTURA + 30 ||
                obstaculosBrancos[i].posicao.y < AREA_JOGO_Y - 30) {
                  // Desativa o obstáculo quando estiver completamente fora da área
                obstaculosBrancos[i].ativo = false;
            }
        }
    }
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

    // Define o retângulo de colisão do coração do jogador
    Rectangle retanguloCoracao = {
        posicaoCoracao.x - 6,
        posicaoCoracao.y - 6,
        12,
        12
    };

    float escalaVertical = 0.35f;
    float alturaVertical = 153 * escalaVertical;
    float larguraVertical = 62 * escalaVertical;

    float alturaHorizontal = 13 * escalaVertical;
    float larguraHorizontal = 62 * escalaVertical;

    // Define os limites da área de jogo para verificação de visibilidade
    float limiteEsquerdo = AREA_JOGO_X;
    float limiteDireito = AREA_JOGO_X + AREA_JOGO_LARGURA;
    float limiteSuperior = AREA_JOGO_Y;
    float limiteInferior = AREA_JOGO_Y + AREA_JOGO_ALTURA;

    for (int i = 0; i < MAX_OBSTACULOS_BRANCOS; i++) {
        if (obstaculosBrancos[i].ativo) {
            // Ignora obstáculos que estão completamente fora da área visível
            if (obstaculosBrancos[i].posicao.x < limiteEsquerdo - 40 || 
                obstaculosBrancos[i].posicao.x > limiteDireito + 40 || 
                obstaculosBrancos[i].posicao.y < limiteSuperior - 40 || 
                obstaculosBrancos[i].posicao.y > limiteInferior + 40) {
                
                // Se um obstáculo estiver muito fora da área, vamos desativá-lo imediatamente
                obstaculosBrancos[i].ativo = false;
                continue;
            }
            
            // Verifica se o obstáculo tem ALGUMA PARTE visível na área de jogo antes de testar colisão
            bool temParteVisivel = false;
            Rectangle retanguloOsso;

            if (obstaculosBrancos[i].comprimento == 10) {
                // Osso vertical
                retanguloOsso = (Rectangle){
                    obstaculosBrancos[i].posicao.x - larguraVertical / 2,
                    obstaculosBrancos[i].posicao.y - alturaVertical / 2,
                    larguraVertical,
                    alturaVertical
                };
                
                // Verifica se pelo menos uma parte do osso está na área visível
                temParteVisivel = !(retanguloOsso.x > limiteDireito || 
                                   retanguloOsso.x + retanguloOsso.width < limiteEsquerdo ||
                                   retanguloOsso.y > limiteInferior || 
                                   retanguloOsso.y + retanguloOsso.height < limiteSuperior);
            } else {
                // Osso horizontal
                retanguloOsso = (Rectangle){
                    obstaculosBrancos[i].posicao.x - larguraHorizontal / 2,
                    obstaculosBrancos[i].posicao.y - alturaHorizontal / 2,
                    larguraHorizontal,
                    alturaHorizontal
                };
                
                // Verifica se pelo menos uma parte do osso está na área visível
                temParteVisivel = !(retanguloOsso.x > limiteDireito || 
                                   retanguloOsso.x + retanguloOsso.width < limiteEsquerdo ||
                                   retanguloOsso.y > limiteInferior || 
                                   retanguloOsso.y + retanguloOsso.height < limiteSuperior);
            }

            // Só testa colisão se o objeto tiver alguma parte visível
            if (temParteVisivel && tempoInvulnerabilidade <= 0.0f && CheckCollisionRecs(retanguloCoracao, retanguloOsso)) {
                vidaCoracao -= 5;
                efeitoDanoTempo = 0.5f;
                ultimoDano = 5.0f;
                adicionarNumeroDano(5, posicaoCoracao, true);
                colisao = true;
                tempoInvulnerabilidade = 0.5f;
            }
        }
    }

    return colisao;
}

void mudarParaFase2(void) {
    velocidadeBase = 3.0f;
    
    limparObstaculosEPrepararProximaFase();
}

void mudarParaFase3(void) {
    velocidadeBase = 4.0f;
    limparObstaculosEPrepararProximaFase();
}

void desenharJogo(void) {
    atualizarEscalaTela();
    BeginDrawing();
    ClearBackground(BLACK);
    
    // Se estivermos no modo charada, desenha a tela de charada
    if (modoCharada) {
        desenharTelaCharada();
        EndDrawing();
        return;
    }
    BeginAreaJogoComEscala();
    // Desenha o fundo da fase atual
    Color corFundo;
    switch (faseAtual) {
        case 1: corFundo = (Color){20, 20, 30, 255}; break;
        case 2: corFundo = (Color){30, 10, 40, 255}; break;
        case 3: corFundo = (Color){40, 5, 5, 255}; break;
        default: corFundo = (Color){0, 0, 0, 255}; break;
    }
    DrawRectangle(AREA_JOGO_X, AREA_JOGO_Y, AREA_JOGO_LARGURA, AREA_JOGO_ALTURA, corFundo);
    // Chamar Sans Fight em todas as fases
    sansFightFase();
    
    if (efeitoDanoTempo > 0.0f) {
        float intensidadeEfeito = efeitoDanoTempo;
        if (intensidadeEfeito > 1.0f) intensidadeEfeito = 1.0f;
        
        Color corEfeito;
        if (ultimoDano > 0) {
            corEfeito = (Color){
                255, 0, 0, (unsigned char)(150 * intensidadeEfeito)
            };
        } else {
            corEfeito = (Color){
                100, 255, 100, (unsigned char)(120 * intensidadeEfeito)
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
    
    atualizarBosses();
    atualizarProjeteis();
    desenharProjeteis();
    desenharBosses();

    desenharCoracao();
    
    desenharNumerosDano();
    
    EndAreaJogoComEscala();
    
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
    
    // Exibir pergunta da charada, se o modo charada estiver ativo
    if (modoCharada) {
        DrawRectangle(AREA_JOGO_X + 10, AREA_JOGO_Y + 50, AREA_JOGO_LARGURA - 20, 100, (Color){0, 0, 0, 180});
        DrawText(charadaAtual.pergunta, AREA_JOGO_X + 20, AREA_JOGO_Y + 60, 20, WHITE);
    }
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

// --- Correção na função desenharObstaculos (apenas trecho alterado) ---
void desenharObstaculos(void) {
    Rectangle srcVertical = {53, 36, 62, 153};      // Fonte para ossos verticais
    Rectangle srcHorizontal = {53, 36, 62, 13};     // Fonte para ossos horizontais (ajustada para horizontal)
    
    float escalaVertical = 0.35f;
    
    // Primeiro, defina os limites da área de jogo para verificação de visibilidade
    float limiteEsquerdo = AREA_JOGO_X;
    float limiteDireito = AREA_JOGO_X + AREA_JOGO_LARGURA;
    float limiteSuperior = AREA_JOGO_Y;
    float limiteInferior = AREA_JOGO_Y + AREA_JOGO_ALTURA;
    
    for (int i = 0; i < MAX_OBSTACULOS_BRANCOS; i++) {
        if (obstaculosBrancos[i].ativo) {
            float xOsso = obstaculosBrancos[i].posicao.x;
            float yOsso = obstaculosBrancos[i].posicao.y;
            
            // Verificar se o obstáculo está pelo menos parcialmente visível
            // antes de tentar desenhá-lo
            Rectangle areaObstaculo;
            
            if (obstaculosBrancos[i].comprimento == 10) {
                // Osso vertical
                float largura = srcVertical.width * escalaVertical;
                float altura = srcVertical.height * escalaVertical;
                areaObstaculo = (Rectangle){
                    xOsso - largura/2, 
                    yOsso - altura/2,
                    largura,
                    altura
                };
                
                // Só desenha se estiver pelo menos parcialmente visível
                if (!(areaObstaculo.x > limiteDireito || 
                      areaObstaculo.x + areaObstaculo.width < limiteEsquerdo ||
                      areaObstaculo.y > limiteInferior || 
                      areaObstaculo.y + areaObstaculo.height < limiteSuperior)) {
                    
                    DrawTexturePro(
                        texturaOssoReto,
                        srcVertical,
                        areaObstaculo,
                        (Vector2){0,0},
                        0,
                        WHITE
                    );
                }
            } else {
                // Osso horizontal
                float largura = srcHorizontal.width * escalaVertical;
                float altura = srcHorizontal.height * escalaVertical;
                areaObstaculo = (Rectangle){
                    xOsso - largura/2, 
                    yOsso - altura/2,
                    largura,
                    altura
                };
                
                // Só desenha se estiver pelo menos parcialmente visível
                if (!(areaObstaculo.x > limiteDireito || 
                      areaObstaculo.x + areaObstaculo.width < limiteEsquerdo ||
                      areaObstaculo.y > limiteInferior || 
                      areaObstaculo.y + areaObstaculo.height < limiteSuperior)) {
                    
                    DrawTexturePro(
                        texturaOssoHorizontal,
                        srcHorizontal,
                        areaObstaculo,
                        (Vector2){0,0},
                        90.0f,  // Rotação para o osso horizontal
                        WHITE
                    );
                }
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
