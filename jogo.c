#include "jogo.h"
#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include <string.h>

// Definições de constantes
#define LARGURA_TELA 800
#define AREA_JOGO_X 50
#define AREA_JOGO_Y 50
#define AREA_JOGO_LARGURA 700
#define AREA_JOGO_ALTURA 500
// Velocidade do coração definida em jogo.h

// Variáveis globais do jogo
Vector2 posicaoCoracao;
float vidaCoracao = 200.0f;
float pontuacao = 0.0f;
int faseAtual = 1;
NivelDificuldade dificuldadeAtual = NORMAL;

// Variáveis locais do jogo
static bool jogadorMovendo;
static int contadorObstaculos = 0;
static float tempoJogo = 0.0f;
static bool modoChefao = false;
static int bossAtual = 0;
static float velocidadeBase = 2.5f;  // Velocidade base dos obstáculos
static float velocidadeCoracao = VELOCIDADE_CORACAO;  // Velocidade do coração

// Variáveis para o efeito visual de dano
float efeitoDanoTempo = 0.0f;  // Removed 'static' to make it accessible
float ultimoDano = 0.0f;       // Removed 'static' to make it accessible
static float vidaAnterior = 200.0f;

// Array de números de dano flutuantes
static NumeroDano numerosDano[MAX_NUMEROS_DANO];
// Texturas e sons (comentados para evitar warnings)
// static Texture2D texturasFundo[3];
// static Sound somDano, somPontos, somBoss, somGameOver, somVitoria;
// static Music musicaFundo;
static bool efeitosVisuaisAvancados = true; // Para desativar em hardware mais fraco

// Arrays de obstáculos
static Obstaculo obstaculosBrancos[MAX_OBSTACULOS_BRANCOS];  
static Obstaculo obstaculosRoxos[MAX_OBSTACULOS_ROXOS];    
static Obstaculo obstaculosAmarelos[MAX_OBSTACULOS_AMARELOS];

// Texturas e recursos
static Texture2D texturaCoracao;
static Texture2D texturaBoss;

// Implementação das funções do jogo

// Inicializa o jogo
void inicializarJogo(void) {
    // Inicializa variáveis do jogo
    posicaoCoracao.x = AREA_JOGO_X + 50;
    posicaoCoracao.y = AREA_JOGO_Y + AREA_JOGO_ALTURA / 2;
    vidaCoracao = 200.0f;
    pontuacao = 0;
    faseAtual = 1;
    jogadorMovendo = false;
    contadorObstaculos = 0;
    velocidadeBase = 2.5f;  // Velocidade base aumentada para maior dificuldade
    velocidadeCoracao = VELOCIDADE_CORACAO;
    modoChefao = true;  // Ativa o modo chefão desde o início
    dificuldadeAtual = NORMAL; // Define a dificuldade inicial
    
    // Inicializa obstáculos
    inicializarObstaculos();
    
    // Inicializa números de dano
    inicializarNumerosDano();
    
    // Inicializa a IA
    inicializarIA();
    
    // Inicializa e ativa o boss da fase 1
    inicializarBosses();
    ativarBossDaFase(1);
    
    // Cria uma imagem com um coração em pixel art mais bonito e detalhado
    Image imagemCoracao = GenImageColor(TAMANHO_CORACAO, TAMANHO_CORACAO, BLANK);
    
    // Definimos a matriz de pixels para o coração com diferentes cores para dar profundidade
    // 0 = transparente, 1 = vermelho escuro (contorno), 2 = vermelho normal, 3 = vermelho claro (destaque)
    int pixelArt[16][16] = {
        {0,0,0,0,1,1,0,0,0,0,1,1,0,0,0,0},
        {0,0,1,1,2,2,1,0,0,1,2,2,1,1,0,0},
        {0,1,2,2,2,2,2,1,1,2,2,2,2,2,1,0},
        {1,2,2,2,2,2,2,2,2,2,2,2,2,2,2,1},
        {1,2,2,2,2,2,2,2,2,2,2,2,2,2,2,1},
        {1,2,2,2,2,2,2,2,2,2,2,2,2,2,2,1},
        {1,2,2,2,2,2,2,2,2,2,2,2,2,2,2,1},
        {1,2,2,2,2,2,2,2,2,2,2,2,2,2,2,1},
        {0,1,2,2,2,2,2,2,2,2,2,2,2,2,1,0},
        {0,0,1,2,2,2,2,2,2,2,2,2,2,1,0,0},
        {0,0,0,1,2,2,2,2,2,2,2,2,1,0,0,0},
        {0,0,0,0,1,2,2,2,2,2,2,1,0,0,0,0},
        {0,0,0,0,0,1,2,2,2,2,1,0,0,0,0,0},
        {0,0,0,0,0,0,1,2,2,1,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}
    };
    
    // Adiciona destaques para dar mais profundidade
    // Posições de destaque (vermelho mais claro)
    int destaques[8][2] = {
        {4, 2}, {5, 2}, {10, 2}, {11, 2},
        {3, 3}, {6, 3}, {9, 3}, {12, 3}
    };
    
    for (int i = 0; i < 8; i++) {
        int x = destaques[i][0];
        int y = destaques[i][1];
        if (pixelArt[y][x] == 2) {
            pixelArt[y][x] = 3;
        }
    }
    
    // Calcula o fator de escala para ajustar ao tamanho desejado
    float escala = (float)TAMANHO_CORACAO / 16.0f;
    
    // Desenha o coração com diferentes tons de vermelho para dar profundidade
    for (int y = 0; y < 16; y++) {
        for (int x = 0; x < 16; x++) {
            if (pixelArt[y][x] > 0) {
                // Define cores com base no valor da matriz
                Color cor;
                switch(pixelArt[y][x]) {
                    case 1: // Contorno - vermelho escuro
                        cor = (Color){180, 0, 0, 255};
                        break;
                    case 2: // Cor principal - vermelho normal
                        cor = (Color){230, 0, 0, 255};
                        break;
                    case 3: // Destaque - vermelho claro
                        cor = (Color){255, 60, 60, 255};
                        break;
                    default:
                        cor = RED;
                }
                
                // Desenha um quadrado escalado para cada pixel da matriz
                for (int py = 0; py < (int)escala + 1; py++) {
                    for (int px = 0; px < (int)escala + 1; px++) {
                        int posX = (int)(x * escala) + px;
                        int posY = (int)(y * escala) + py;
                        if (posX < TAMANHO_CORACAO && posY < TAMANHO_CORACAO) {
                            ImageDrawPixel(&imagemCoracao, posX, posY, cor);
                        }
                    }
                }
            }
        }
    }
    
    texturaCoracao = LoadTextureFromImage(imagemCoracao);
    UnloadImage(imagemCoracao);
    
    // Inicializa o coração e obstáculos
    inicializarCoracao();
    inicializarObstaculos();
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

// Atualiza o estado do jogo
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
    
    if (pontuacao >= 600 && faseAtual == 3) {
        return false; // Jogo vencido
    }
    
    // Atualiza o coração
    atualizarCoracao();
    
    // Gera e atualiza obstáculos com frequência aumentada baseada na fase
    contadorObstaculos++;
    int intervaloAjustado = INTERVALO_GERACAO_OBSTACULO - (faseAtual * 5); // Mais rápido em fases avançadas
    if (intervaloAjustado < 5) intervaloAjustado = 5; // Limite mínimo
    
    // Gera novos obstáculos aleatoriamente com frequência baseada na dificuldade
    // Aumentado para maior velocidade
    int chanceObstaculo = 10; // Dobrado para mais obstáculos
    if (dificuldadeAtual == DIFICIL) chanceObstaculo = 15;
    if (dificuldadeAtual == CUPHEAD) chanceObstaculo = 20;
    
    if (GetRandomValue(0, 100) < chanceObstaculo) {
        gerarObstaculoBranco();
    }
    
    // Gera obstáculos roxos com menor frequência
    int chanceRoxo = (faseAtual >= 2) ? 4 : 2; // Aumentado para mais obstáculos roxos
    if (dificuldadeAtual == DIFICIL) chanceRoxo += 3;
    if (dificuldadeAtual == CUPHEAD) chanceRoxo += 5;
    
    if (GetRandomValue(0, 100) < chanceRoxo) {
        gerarObstaculoRoxo();
    }
    
    // Gera obstáculos amarelos raramente (recuperam vida)
    // Mais raros em dificuldades maiores
    int chanceAmarelo = 5; // Aumentado para mais chances de recuperar vida
    if (dificuldadeAtual == DIFICIL) chanceAmarelo = 3;
    if (dificuldadeAtual == CUPHEAD) chanceAmarelo = 2;
    
    if (GetRandomValue(0, 200) < chanceAmarelo) {
        gerarObstaculoAmarelo();
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
        // Game Over - return false to trigger game over screen
        return false;
    }
    
    // Teclas especiais
    if (IsKeyPressed(KEY_F1)) {
        // Alterna entre os níveis de dificuldade
        dificuldadeAtual = (dificuldadeAtual + 1) % 4;
        definirDificuldade(dificuldadeAtual);
    }
    
    if (IsKeyPressed(KEY_F2)) {
        // Ativa/desativa efeitos visuais avançados
        efeitosVisuaisAvancados = !efeitosVisuaisAvancados;
    }
    
    return true;
}

// Atualiza o coração
void atualizarCoracao(void) {
    // Movimento para cima
    if (IsKeyDown(KEY_W) && posicaoCoracao.y > AREA_JOGO_Y + TAMANHO_CORACAO/2) {
        posicaoCoracao.y -= velocidadeCoracao;
        jogadorMovendo = true;
    }
    
    // Movimento para baixo
    if (IsKeyDown(KEY_S) && posicaoCoracao.y < AREA_JOGO_Y + AREA_JOGO_ALTURA - TAMANHO_CORACAO/2) {
        posicaoCoracao.y += velocidadeCoracao;
        jogadorMovendo = true;
    }
    
    // Movimento para a esquerda
    if (IsKeyDown(KEY_A) && posicaoCoracao.x > AREA_JOGO_X + TAMANHO_CORACAO/2) {
        posicaoCoracao.x -= velocidadeCoracao;
        jogadorMovendo = true;
    }
    
    // Movimento para a direita
    if (IsKeyDown(KEY_D) && posicaoCoracao.x < AREA_JOGO_X + AREA_JOGO_LARGURA - TAMANHO_CORACAO/2) {
        posicaoCoracao.x += velocidadeCoracao;
        jogadorMovendo = true;
    }
}

void gerarObstaculoBranco(void) {
    for (int i = 0; i < 50; i++) {
        if (!obstaculosBrancos[i].ativo) {
            obstaculosBrancos[i].ativo = true;
            obstaculosBrancos[i].posicao.x = AREA_JOGO_X + AREA_JOGO_LARGURA;
            obstaculosBrancos[i].posicao.y = AREA_JOGO_Y + GetRandomValue(0, AREA_JOGO_ALTURA - 30);
            
            // Tamanhos mais variados
            obstaculosBrancos[i].comprimento = GetRandomValue(10, 40);
            
            // Velocidade mais alta e variada
            obstaculosBrancos[i].velocidade = velocidadeBase * 1.5f + GetRandomValue(0, 200) / 100.0f;
            
            // Alguns obstáculos são mais rápidos e direcionados ao jogador
            if (GetRandomValue(0, 10) < 3) {
                obstaculosBrancos[i].velocidade *= 1.5f;
                // Direciona mais para a posição do jogador
                obstaculosBrancos[i].posicao.y = posicaoCoracao.y + GetRandomValue(-50, 50);
            }
            
            break;
        }
    }
}

// Gera um novo obstáculo roxo
void gerarObstaculoRoxo(void) {
    // Apenas gera obstáculos roxos a partir da fase 2
    if (faseAtual < 2) return;
    
    // Controle de frequência para não gerar muitos obstáculos roxos
    if (GetRandomValue(0, 100) < 30) {
        for (int i = 0; i < 30; i++) {
            if (!obstaculosRoxos[i].ativo) {
                obstaculosRoxos[i].ativo = true;
                obstaculosRoxos[i].posicao.x = AREA_JOGO_X + AREA_JOGO_LARGURA;
                obstaculosRoxos[i].posicao.y = AREA_JOGO_Y + GetRandomValue(40, AREA_JOGO_ALTURA - 40);
                obstaculosRoxos[i].comprimento = GetRandomValue(2, 4) * 10; // Definindo o comprimento
                obstaculosRoxos[i].velocidade = velocidadeBase * 0.8f;
                break;
            }
        }
    }
}

// Gera um novo obstáculo amarelo (recupera menos vida)
void gerarObstaculoAmarelo(void) {
    // Apenas gera obstáculos amarelos na fase 3
    if (faseAtual < 3) return;
    
    // Controle de frequência para não gerar muitos obstáculos amarelos
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

// Atualiza os obstáculos brancos
void atualizarObstaculosBrancos(void) {
    for (int i = 0; i < 50; i++) {
        if (obstaculosBrancos[i].ativo) {
            // Move o obstáculo para a esquerda com velocidade variável
            obstaculosBrancos[i].posicao.x -= obstaculosBrancos[i].velocidade;
            
            // Movimento em zig-zag para aumentar a dificuldade
            if (faseAtual >= 2) {
                float tempo = GetTime();
                obstaculosBrancos[i].posicao.y += sinf(tempo * 3.0f + i) * 1.5f;
                
                // Mantém dentro dos limites da área de jogo
                if (obstaculosBrancos[i].posicao.y < AREA_JOGO_Y + 10) {
                    obstaculosBrancos[i].posicao.y = AREA_JOGO_Y + 10;
                }
                if (obstaculosBrancos[i].posicao.y > AREA_JOGO_Y + AREA_JOGO_ALTURA - 10 - 20) { // Usando valor fixo de 20 para altura
                    obstaculosBrancos[i].posicao.y = AREA_JOGO_Y + AREA_JOGO_ALTURA - 10 - 20;
                }
            }
            
            // Na fase 3, alguns obstáculos perseguem o jogador
            if (faseAtual >= 3 && i % 3 == 0) {
                // Movimento de perseguição suave em direção ao jogador
                if (obstaculosBrancos[i].posicao.y < posicaoCoracao.y) {
                    obstaculosBrancos[i].posicao.y += 1.0f;
                } else if (obstaculosBrancos[i].posicao.y > posicaoCoracao.y) {
                    obstaculosBrancos[i].posicao.y -= 1.0f;
                }
            }
            
            // Desativa o obstáculo se sair da área de jogo
            if (obstaculosBrancos[i].posicao.x < AREA_JOGO_X) {
                obstaculosBrancos[i].ativo = false;
            }
        }
    }
}

// Atualiza os obstáculos roxos
void atualizarObstaculosRoxos(void) {
    for (int i = 0; i < 30; i++) {
        if (obstaculosRoxos[i].ativo) {
            // Move o obstáculo para a esquerda
            obstaculosRoxos[i].posicao.x -= obstaculosRoxos[i].velocidade;
            
            // Desativa o obstáculo se sair da área de jogo
            if (obstaculosRoxos[i].posicao.x < AREA_JOGO_X) {
                obstaculosRoxos[i].ativo = false;
            }
        }
    }
}

// Atualiza os obstáculos amarelos
void atualizarObstaculosAmarelos(void) {
    for (int i = 0; i < MAX_OBSTACULOS_AMARELOS; i++) {
        if (obstaculosAmarelos[i].ativo) {
            // Movimento senoidal mais complexo
            obstaculosAmarelos[i].posicao.x -= obstaculosAmarelos[i].velocidade;
            
            // Amplitude maior para movimento vertical
            float amplitude = 80.0f;
            float frequencia = 0.05f;
            
            // Movimento senoidal baseado na posição x e no tempo
            obstaculosAmarelos[i].posicao.y = AREA_JOGO_Y + AREA_JOGO_ALTURA/2 + 
                                            amplitude * sinf(obstaculosAmarelos[i].posicao.x * frequencia + 
                                                             obstaculosAmarelos[i].angulo);
            
            // Rotação dinâmica
            obstaculosAmarelos[i].rotacao += 2.0f;
            
            // Desativa se sair da área de jogo
            if (obstaculosAmarelos[i].posicao.x < AREA_JOGO_X - 50) {
                obstaculosAmarelos[i].ativo = false;
            }
        }
    }
}

// Função para atualizar todos os obstáculos
void atualizarObstaculos(void) {
    atualizarObstaculosBrancos();
    atualizarObstaculosRoxos();
    atualizarObstaculosAmarelos();
}

// Função para atualizar a fase atual do jogo
void atualizarFase(void) {
    // Muda para a fase 2 quando a pontuação atinge 1000
    if (faseAtual == 1 && pontuacao >= 1000.0f) {
        mudarParaFase2();
    }
    
    // Muda para a fase 3 quando a pontuação atinge 2500
    if (faseAtual == 2 && pontuacao >= 2500.0f) {
        mudarParaFase3();
    }
}

// Detecta colisões entre o coração e os obstáculos
bool detectarColisoes(void) {
    bool colisao = false;
    Rectangle coracaoRect = (Rectangle){posicaoCoracao.x, posicaoCoracao.y, TAMANHO_CORACAO, TAMANHO_CORACAO};
    
    // Verifica colisões com obstáculos brancos (dano aumentado)
    for (int i = 0; i < 50; i++) {
        if (obstaculosBrancos[i].ativo) {
            Rectangle obstaculoRect = (Rectangle){
                obstaculosBrancos[i].posicao.x,
                obstaculosBrancos[i].posicao.y,
                10,
                20 // Altura fixa do obstáculo
            };
            
            if (CheckCollisionRecs(coracaoRect, obstaculoRect)) {
                obstaculosBrancos[i].ativo = false;
                // Guarda o valor anterior da vida
                vidaAnterior = vidaCoracao;
                
                // Dano aumentado
                float danoCausado = 15.0f + faseAtual * 5.0f;
                vidaCoracao -= danoCausado;
                
                // Atualiza variáveis para efeito visual de dano
                ultimoDano = vidaAnterior - vidaCoracao;
                efeitoDanoTempo = 1.0f; // Duração do efeito em segundos
                
                // Cria número de dano flutuante
                adicionarNumeroDano(danoCausado, posicaoCoracao, true);
                
                // Efeito visual de dano
                for (int j = 0; j < 10; j++) {
                    // Aqui poderia adicionar partículas ou outros efeitos visuais
                }
                
                // Penalidade na pontuação
                pontuacao -= 20;
                if (pontuacao < 0) pontuacao = 0;
                
                colisao = true;
            }
        }
    }
    
    // Verifica colisões com obstáculos roxos (dano severo)
    for (int i = 0; i < 30; i++) {
        if (obstaculosRoxos[i].ativo) {
            Rectangle obstaculoRect = (Rectangle){
                obstaculosRoxos[i].posicao.x,
                obstaculosRoxos[i].posicao.y,
                10,
                20 // Altura fixa do obstáculo
            };
            
            if (CheckCollisionRecs(coracaoRect, obstaculoRect)) {
                obstaculosRoxos[i].ativo = false;
                // Guarda o valor anterior da vida
                vidaAnterior = vidaCoracao;
                
                // Dano severo
                float danoCausado = 25.0f + faseAtual * 7.0f;
                vidaCoracao -= danoCausado;
                
                // Atualiza variáveis para efeito visual de dano
                ultimoDano = vidaAnterior - vidaCoracao;
                efeitoDanoTempo = 1.5f; // Duração maior do efeito para dano severo
                
                // Cria número de dano flutuante maior e mais visível
                adicionarNumeroDano(danoCausado, posicaoCoracao, true);
                
                // Efeito de desaceleração temporária do jogador
                velocidadeCoracao *= 0.7f;
                
                // Penalidade maior na pontuação
                pontuacao -= 50;
                if (pontuacao < 0) pontuacao = 0;
                
                colisao = true;
            }
        }
    }
    
    // Verifica colisões com obstáculos amarelos (recupera menos vida)
    for (int i = 0; i < 10; i++) {
        if (obstaculosAmarelos[i].ativo) {
            Rectangle obstaculoRect = (Rectangle){
                obstaculosAmarelos[i].posicao.x,
                obstaculosAmarelos[i].posicao.y,
                15,
                15  // Tamanho fixo para obstáculos amarelos
            };
            
            if (CheckCollisionRecs(coracaoRect, obstaculoRect)) {
                obstaculosAmarelos[i].ativo = false;
                // Guarda o valor anterior da vida
                vidaAnterior = vidaCoracao;
                
                // Recupera menos vida, tornando mais difícil se manter vivo
                float curaObtida = 10.0f;
                vidaCoracao += curaObtida;
                if (vidaCoracao > 200.0f) vidaCoracao = 200.0f;
                
                // Atualiza variáveis para efeito visual de cura
                ultimoDano = vidaCoracao - vidaAnterior; // Valor positivo para cura
                efeitoDanoTempo = 0.8f; // Duração do efeito de cura
                
                // Cria número de cura flutuante
                adicionarNumeroDano(curaObtida, posicaoCoracao, false);
                
                // Bônus de pontuação
                pontuacao += 100;
            }
        }
    }
    
    return colisao;
}

// Muda para a fase 2
void mudarParaFase2(void) {
    velocidadeBase = 3.0f;
    
    // Limpa todos os obstáculos ativos
    for (int i = 0; i < 50; i++) {
        obstaculosBrancos[i].ativo = false;
    }
    
    // Cria uma imagem de boss assustador em pixel art para a fase 2
    Image imagemBoss = GenImageColor(100, 100, BLANK);
    
    // Cores para o pixel art
    Color corPrincipal = (Color){75, 0, 130, 255};  // Roxo escuro
    Color corSecundaria = (Color){148, 0, 211, 255}; // Roxo mais claro
    Color corOlhos = (Color){255, 0, 0, 255};        // Vermelho para os olhos
    Color corBoca = (Color){0, 0, 0, 255};           // Preto para a boca
    
    // Desenha o corpo principal do boss
    for (int y = 0; y < 100; y++) {
        for (int x = 0; x < 100; x++) {
            // Forma básica do boss (uma forma irregular e assustadora)
            if ((x > 20 && x < 80 && y > 10 && y < 90) || 
                (x > 10 && x < 90 && y > 30 && y < 70)) {
                
                // Adiciona textura e detalhes
                if ((x + y) % 7 == 0 || (x * y) % 13 == 0) {
                    ImageDrawPixel(&imagemBoss, x, y, corSecundaria);
                } else {
                    ImageDrawPixel(&imagemBoss, x, y, corPrincipal);
                }
            }
        }
    }
    
    // Desenha olhos assustadores (grandes e vermelhos com pupilas pretas)
    for (int y = 30; y < 50; y++) {
        for (int x = 30; x < 45; x++) {
            if (sqrt(pow(x - 37, 2) + pow(y - 40, 2)) < 8) {
                ImageDrawPixel(&imagemBoss, x, y, corOlhos);
                // Pupila
                if (sqrt(pow(x - 37, 2) + pow(y - 40, 2)) < 3) {
                    ImageDrawPixel(&imagemBoss, x, y, BLACK);
                }
            }
        }
    }
    
    for (int y = 30; y < 50; y++) {
        for (int x = 55; x < 70; x++) {
            if (sqrt(pow(x - 62, 2) + pow(y - 40, 2)) < 8) {
                ImageDrawPixel(&imagemBoss, x, y, corOlhos);
                // Pupila
                if (sqrt(pow(x - 62, 2) + pow(y - 40, 2)) < 3) {
                    ImageDrawPixel(&imagemBoss, x, y, BLACK);
                }
            }
        }
    }
    
    // Desenha boca assustadora (cheia de dentes afiados)
    for (int y = 60; y < 80; y++) {
        for (int x = 35; x < 65; x++) {
            if (y > 60 && y < 75 && x > 35 && x < 65) {
                if (y % 5 == 0 || x % 5 == 0) {
                    // Dentes afiados
                    ImageDrawPixel(&imagemBoss, x, y, WHITE);
                } else {
                    // Interior da boca
                    ImageDrawPixel(&imagemBoss, x, y, corBoca);
                }
            }
        }
    }
    
    // Adiciona detalhes assustadores (chifres, espinhos, etc.)
    for (int i = 0; i < 20; i++) {
        for (int j = 0; j < i/2; j++) {
            ImageDrawPixel(&imagemBoss, 40 + i, 10 - j, corSecundaria);
            ImageDrawPixel(&imagemBoss, 60 - i, 10 - j, corSecundaria);
        }
    }
    
    texturaBoss = LoadTextureFromImage(imagemBoss);
    UnloadImage(imagemBoss);
}

// Muda para a fase 3
void mudarParaFase3(void) {
    velocidadeBase = 4.0f;
    
    // Limpa todos os obstáculos ativos
    for (int i = 0; i < 50; i++) {
        obstaculosBrancos[i].ativo = false;
    }
    
    for (int i = 0; i < 30; i++) {
        obstaculosRoxos[i].ativo = false;
    }
    
    // Atualiza a textura do boss para a fase 3
    UnloadTexture(texturaBoss);
    
    // Cria uma imagem de boss final ainda mais assustador em pixel art
    Image imagemBoss = GenImageColor(120, 120, BLANK);
    
    // Cores para o pixel art
    Color corPrincipal = (Color){139, 0, 0, 255};    // Vermelho escuro (sangue seco)
    Color corSecundaria = (Color){220, 20, 60, 255};  // Vermelho mais vivo
    Color corDetalhes = (Color){25, 25, 25, 255};     // Quase preto para sombras
    Color corOlhos = (Color){255, 255, 0, 255};       // Amarelo brilhante para os olhos
    
    // Desenha o corpo principal do boss (forma irregular e mais complexa)
    for (int y = 0; y < 120; y++) {
        for (int x = 0; x < 120; x++) {
            // Forma básica do boss (uma forma irregular e muito assustadora)
            float distCentro = sqrt(pow(x - 60, 2) + pow(y - 60, 2));
            float angulo = atan2f(y - 60, x - 60);
            
            // Cria uma forma irregular com "tentáculos" e protuberâncias
            float raioBase = 50.0f;
            float raioVariacao = 20.0f * sinf(angulo * 8.0f) + 10.0f * cosf(angulo * 5.0f);
            float raioFinal = raioBase + raioVariacao;
            
            if (distCentro < raioFinal) {
                // Adiciona textura e detalhes
                if ((int)(distCentro + angulo * 10.0f) % 7 == 0) {
                    ImageDrawPixel(&imagemBoss, x, y, corSecundaria);
                } else if ((int)(distCentro * sinf(angulo * 3.0f)) % 5 == 0) {
                    ImageDrawPixel(&imagemBoss, x, y, corDetalhes);
                } else {
                    ImageDrawPixel(&imagemBoss, x, y, corPrincipal);
                }
            }
        }
    }
    
    // Desenha múltiplos olhos assustadores (pequenos e brilhantes)
    int numOlhos = 7;
    for (int i = 0; i < numOlhos; i++) {
        float angulo = (float)i / numOlhos * 2.0f * PI;
        int centroX = 60 + (int)(30.0f * cosf(angulo));
        int centroY = 60 + (int)(30.0f * sinf(angulo));
        int raioOlho = 5 + i % 3;
        
        for (int y = centroY - raioOlho; y <= centroY + raioOlho; y++) {
            for (int x = centroX - raioOlho; x <= centroX + raioOlho; x++) {
                if (x >= 0 && x < 120 && y >= 0 && y < 120) {
                    float distOlho = sqrt(pow(x - centroX, 2) + pow(y - centroY, 2));
                    if (distOlho < raioOlho) {
                        ImageDrawPixel(&imagemBoss, x, y, corOlhos);
                        // Pupila vertical (como de réptil)
                        if (abs(x - centroX) < 2) {
                            ImageDrawPixel(&imagemBoss, x, y, BLACK);
                        }
                    }
                }
            }
        }
    }
    
    // Desenha boca central assustadora (um abismo com dentes)
    for (int y = 50; y < 90; y++) {
        for (int x = 40; x < 80; x++) {
            float distCentro = sqrt(pow(x - 60, 2) + pow(y - 70, 2));
            if (distCentro < 20) {
                // Interior da boca (abismo)
                ImageDrawPixel(&imagemBoss, x, y, BLACK);
                
                // Dentes afiados ao redor
                if (distCentro > 15 && distCentro < 20) {
                    if ((int)(atan2f(y - 70, x - 60) * 10.0f) % 2 == 0) {
                        ImageDrawPixel(&imagemBoss, x, y, WHITE);
                    }
                }
            }
        }
    }
    
    // Adiciona detalhes de sangue e gore
    for (int i = 0; i < 200; i++) {
        int x = GetRandomValue(0, 119);
        int y = GetRandomValue(0, 119);
        float distCentro = sqrt(pow(x - 60, 2) + pow(y - 60, 2));
        
        if (distCentro < 60 && GetRandomValue(0, 100) < 30) {
            Color corSangue = (Color){255, GetRandomValue(0, 30), GetRandomValue(0, 30), 255};
            ImageDrawPixel(&imagemBoss, x, y, corSangue);
            
            // Respingos de sangue
            for (int j = 0; j < 3; j++) {
                int respX = x + GetRandomValue(-3, 3);
                int respY = y + GetRandomValue(-3, 3);
                if (respX >= 0 && respX < 120 && respY >= 0 && respY < 120) {
                    ImageDrawPixel(&imagemBoss, respX, respY, corSangue);
                }
            }
        }
    }
    
    texturaBoss = LoadTextureFromImage(imagemBoss);
    UnloadImage(imagemBoss);
}

// Função para desenhar o jogo
void desenharJogo(void) {
    BeginDrawing();
    ClearBackground(BLACK);
    
    // Desenha o fundo da fase atual
    Color corFundo;
    switch (faseAtual) {
        case 1: corFundo = (Color){20, 20, 30, 255}; break; // Azul escuro
        case 2: corFundo = (Color){30, 10, 40, 255}; break; // Roxo escuro
        case 3: corFundo = (Color){40, 5, 5, 255}; break;   // Vermelho escuro
        default: corFundo = (Color){0, 0, 0, 255}; break;    // Preto
    }
    
    // Desenha a área de jogo com gradiente baseado na fase
    DrawRectangle(AREA_JOGO_X, AREA_JOGO_Y, AREA_JOGO_LARGURA, AREA_JOGO_ALTURA, corFundo);
    
    // Efeito visual de dano/cura
    if (efeitoDanoTempo > 0.0f) {
        float intensidadeEfeito = efeitoDanoTempo;
        if (intensidadeEfeito > 1.0f) intensidadeEfeito = 1.0f;
        
        // Determina se é dano (vermelho) ou cura (verde)
        Color corEfeito;
        if (ultimoDano > 0) { // É dano
            // Vermelho para dano
            corEfeito = (Color){
                255, 
                0, 
                0, 
                (unsigned char)(150 * intensidadeEfeito)
            };
            
            // Vinheta para danos grandes (mais de 20 pontos de vida)
            if (ultimoDano > 20.0f) {
                // Efeito de vinheta (bordas escuras) para danos grandes
                DrawRectangleGradientV(
                    AREA_JOGO_X, 
                    AREA_JOGO_Y, 
                    AREA_JOGO_LARGURA, 
                    AREA_JOGO_ALTURA / 4,
                    (Color){0, 0, 0, (unsigned char)(200 * intensidadeEfeito)},
                    (Color){0, 0, 0, 0}
                );
                
                DrawRectangleGradientV(
                    AREA_JOGO_X, 
                    AREA_JOGO_Y + AREA_JOGO_ALTURA * 3/4, 
                    AREA_JOGO_LARGURA, 
                    AREA_JOGO_ALTURA / 4,
                    (Color){0, 0, 0, 0},
                    (Color){0, 0, 0, (unsigned char)(200 * intensidadeEfeito)}
                );
                
                DrawRectangleGradientH(
                    AREA_JOGO_X, 
                    AREA_JOGO_Y, 
                    AREA_JOGO_LARGURA / 4, 
                    AREA_JOGO_ALTURA,
                    (Color){0, 0, 0, (unsigned char)(200 * intensidadeEfeito)},
                    (Color){0, 0, 0, 0}
                );
                
                DrawRectangleGradientH(
                    AREA_JOGO_X + AREA_JOGO_LARGURA * 3/4, 
                    AREA_JOGO_Y, 
                    AREA_JOGO_LARGURA / 4, 
                    AREA_JOGO_ALTURA,
                    (Color){0, 0, 0, 0},
                    (Color){0, 0, 0, (unsigned char)(200 * intensidadeEfeito)}
                );
            }
        } else { // É cura (ultimoDano negativo)
            // Verde suave para cura
            corEfeito = (Color){
                100, 
                255, 
                100, 
                (unsigned char)(120 * intensidadeEfeito)
            };
            
            // Efeito de brilho para cura
            DrawRectangleGradientV(
                AREA_JOGO_X, 
                AREA_JOGO_Y, 
                AREA_JOGO_LARGURA, 
                AREA_JOGO_ALTURA,
                (Color){200, 255, 200, (unsigned char)(50 * intensidadeEfeito)},
                (Color){100, 255, 100, (unsigned char)(30 * intensidadeEfeito)}
            );
        }
        
        // Desenha retângulo com a cor do efeito sobre toda a área de jogo
        DrawRectangle(
            AREA_JOGO_X, 
            AREA_JOGO_Y, 
            AREA_JOGO_LARGURA, 
            AREA_JOGO_ALTURA, 
            corEfeito
        );
    }
    
    // Efeito de borda iluminada para a área de jogo
    Color corBorda = WHITE;
    if (modoChefao) {
        // No modo chefão, a borda pulsa em vermelho
        float intensidade = 0.5f + 0.5f * sinf(tempoJogo * 5.0f);
        corBorda = (Color){255, (unsigned char)(50 * intensidade), (unsigned char)(50 * intensidade), 255};
    }
    DrawRectangleLines(AREA_JOGO_X, AREA_JOGO_Y, AREA_JOGO_LARGURA, AREA_JOGO_ALTURA, corBorda);
    
    // Desenha partículas de fundo se os efeitos visuais avançados estiverem ativados
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
    
    // Desenha os obstáculos
    desenharObstaculos();
    
    // Se estiver no modo chefão, desenha os projéteis e os bosses
    if (modoChefao) {
        desenharProjeteis();
        desenharBosses();
    }
    
    // Desenha o coração
    desenharCoracao();
    
    // Desenha os números de dano flutuantes
    desenharNumerosDano();
    
    // Desenha a barra de HP na parte inferior da tela (centralizada)
    int barraHPLargura = 200;
    int barraHPAltura = 30;
    int barraHPX = AREA_JOGO_X + (AREA_JOGO_LARGURA - barraHPLargura) / 2;
    int barraHPY = AREA_JOGO_Y + AREA_JOGO_ALTURA + 15;
    
    // Fundo da barra com gradiente
    DrawRectangleGradientH(
        barraHPX, 
        barraHPY, 
        barraHPLargura, 
        barraHPAltura, 
        (Color){60, 0, 0, 220}, 
        (Color){120, 0, 0, 220}
    );
    
    // Barra de vida atual com efeito pulsante
    float pulsacaoBarra = 1.0f + 0.05f * sinf(tempoJogo * 4.0f);
    Color corBarraHP = (Color){220, 0, 0, 220};
    
    // Cores dinâmicas baseadas na quantidade de vida
    if (vidaCoracao < 50) {
        // Vida baixa: vermelho pulsante
        corBarraHP.r = 220 + (unsigned char)(35 * sinf(tempoJogo * 10.0f));
        corBarraHP.g = 0;
        pulsacaoBarra = 1.0f + 0.1f * sinf(tempoJogo * 8.0f); // Pulsação mais rápida
    } else if (vidaCoracao < 100) {
        // Vida média: alaranjado
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
    
    // Borda da barra com brilho
    Color corBordaBarra = WHITE;
    if (vidaCoracao < 50) {
        // Borda piscante para vida baixa
        corBordaBarra.a = 128 + (unsigned char)(127 * sinf(tempoJogo * 8.0f));
    }
    DrawRectangleLines(barraHPX, barraHPY, barraHPLargura, barraHPAltura, corBordaBarra);
    
    // Texto HP com sombra para melhor legibilidade
    char textoHP[20];
    sprintf(textoHP, "HP: %d/200", (int)vidaCoracao);
    int textoHPLargura = MeasureText(textoHP, 20);
    
    // Sombra
    DrawText(textoHP, barraHPX + (barraHPLargura - textoHPLargura) / 2 + 1, barraHPY + 6 + 1, 20, BLACK);
    // Texto principal
    DrawText(textoHP, barraHPX + (barraHPLargura - textoHPLargura) / 2, barraHPY + 6, 20, WHITE);
    
    // Desenha informações da fase atual em posição não sobreposta
    {
        char textoFase[50];
        
        switch(faseAtual) {
            case 1: sprintf(textoFase, "FASE 1: LEMBRANÇAS"); break;
            case 2: sprintf(textoFase, "FASE 2: PESADELOS"); break;
            case 3: sprintf(textoFase, "FASE 3: CONFRONTO FINAL"); break;
            default: sprintf(textoFase, "FASE %d", faseAtual); break;
        }
        
        // Cria um fundo escuro para o texto da fase para melhor legibilidade
        DrawRectangle(AREA_JOGO_X, 8, MeasureText(textoFase, 20) + 10, 24, (Color){0, 0, 0, 180});
        DrawText(textoFase, AREA_JOGO_X + 5, 10, 20, WHITE);
    }
    
    // Informações do boss (canto superior direito)
    char textoBoss[50] = "";
    char textoPontuacao[50];
    
    // Determine o texto do boss baseado na fase atual
    if (faseAtual >= 1 && faseAtual <= 3) {
        switch (faseAtual) {
            case 1: sprintf(textoBoss, "Boss: Memória Dolorosa"); break;
            case 2: sprintf(textoBoss, "Boss: Trauma Profundo"); break;
            case 3: sprintf(textoBoss, "Boss: Emoção Caótica"); break;
            default: textoBoss[0] = '\0';
        }
    }
    
    // Texto da pontuação
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
    
    // Pontuação (lado esquerdo, abaixo do HP)
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

// Desenha o coração vermelho com efeitos visuais sutis
void desenharCoracao(void) {
    static float tempo = 0.0f;
    tempo += GetFrameTime();
    
    // Efeito de pulsação sutil
    float escala = 1.0f + 0.05f * sinf(tempo * 2.5f);
    
    // Rotação muito sutil para dar vida ao coração
    float rotacao = 2.0f * sinf(tempo * 1.5f);
    
    // Calcula o centro do coração para transformações
    Vector2 centro = { TAMANHO_CORACAO / 2.0f, TAMANHO_CORACAO / 2.0f };
    
    // Efeito de tremor quando a vida está baixa
    float offsetX = 0.0f;
    float offsetY = 0.0f;
    if (vidaCoracao < 50.0f) {
        offsetX = GetRandomValue(-1, 1);
        offsetY = GetRandomValue(-1, 1);
        // Pulsação mais rápida quando está com pouca vida
        escala = 1.0f + 0.08f * sinf(tempo * 6.0f);
    }
    
    // Cor base do coração - vermelho brilhante
    Color corBase = WHITE; // Usa branco para manter as cores originais da textura
    
    // Desenha o coração com efeitos sutis
    DrawTexturePro(
        texturaCoracao,
        (Rectangle){ 0, 0, texturaCoracao.width, texturaCoracao.height },
        (Rectangle){ 
            posicaoCoracao.x + offsetX + TAMANHO_CORACAO / 2, 
            posicaoCoracao.y + offsetY + TAMANHO_CORACAO / 2, 
            texturaCoracao.width * escala, 
            texturaCoracao.height * escala 
        },
        centro,
        rotacao,
        corBase
    );
    
    // Efeito de rastro quando o coração se move
    if (jogadorMovendo) {
        static Vector2 posicoesAnteriores[3] = {0};
        static int indiceRastro = 0;
        
        // Atualiza as posições anteriores
        posicoesAnteriores[indiceRastro] = posicaoCoracao;
        indiceRastro = (indiceRastro + 1) % 3;
        
        // Desenha o rastro
        for (int i = 0; i < 3; i++) {
            if (posicoesAnteriores[i].x > 0 && posicoesAnteriores[i].y > 0) {
                int idx = (indiceRastro + i) % 3;
                float alpha = 0.4f - (float)i / 3.0f * 0.3f;
                
                Color corRastro = (Color){
                    255,  // R
                    255,  // G
                    255,  // B
                    (unsigned char)(alpha * 255)
                };
                
                float escalaRastro = 0.95f - (float)i * 0.1f;
                
                DrawTexturePro(
                    texturaCoracao,
                    (Rectangle){ 0, 0, texturaCoracao.width, texturaCoracao.height },
                    (Rectangle){ 
                        posicoesAnteriores[idx].x + TAMANHO_CORACAO / 2, 
                        posicoesAnteriores[idx].y + TAMANHO_CORACAO / 2, 
                        texturaCoracao.width * escalaRastro, 
                        texturaCoracao.height * escalaRastro 
                    },
                    centro,
                    rotacao,
                    corRastro
                );
            }
        }
    }
    
    // Adiciona pequenos brilhos ao redor do coração
    int numParticulas = (vidaCoracao > 150.0f) ? 4 : (vidaCoracao > 100.0f) ? 2 : 0;
    
    for (int i = 0; i < numParticulas; i++) {
        if (GetRandomValue(0, 100) < 30) {
            float angulo = GetRandomValue(0, 360) * DEG2RAD;
            float distancia = GetRandomValue(2, 6);
            
            float px = posicaoCoracao.x + TAMANHO_CORACAO/2 + distancia * cosf(angulo);
            float py = posicaoCoracao.y + TAMANHO_CORACAO/2 + distancia * sinf(angulo);
            
            Color corBrilho = (Color){255, 200, 200, 150 + GetRandomValue(0, 50)};
            DrawCircleV((Vector2){px, py}, 1.0f, corBrilho);
        }
    }
}
 
// Desenha os obstáculos
void desenharObstaculos(void) {
    float tempo = GetTime();
    
    // Desenha obstáculos brancos (ossos como em Undertale, com melhorias visuais)
    for (int i = 0; i < MAX_OBSTACULOS_BRANCOS; i++) {
        if (obstaculosBrancos[i].ativo) {
            // Posição e dimensões do osso (aumentado e melhor proporcionado)
            float comprimentoOsso = 40.0f;
            float larguraOsso = 12.0f;
            float xOsso = obstaculosBrancos[i].posicao.x;
            float yOsso = obstaculosBrancos[i].posicao.y;
            
            // Tamanho das extremidades (aumentado para dar mais equilíbrio visual)
            float raioExtremidade = larguraOsso/2 + 4.0f;
            
            // Cores melhoradas para dar profundidade
            Color corOssoPrincipal = (Color){255, 255, 255, 255}; // Branco puro
            Color corOssoInterior = (Color){240, 240, 245, 255}; // Branco levemente azulado
            Color corSombra = (Color){200, 200, 215, 255}; // Sombra com tom azulado suave
            Color corBrilho = (Color){255, 255, 255, 255}; // Brilho
            
            // Desenha SOMBRA do osso inteiro (para dar sensação de elevação)
            DrawRectangle(
                xOsso + 3, 
                yOsso + larguraOsso + 7, 
                comprimentoOsso, 
                larguraOsso/4, 
                (Color){20, 20, 20, 100}
            );
            DrawCircle(
                xOsso, 
                yOsso + 5 + larguraOsso/2, 
                raioExtremidade + 1, 
                (Color){20, 20, 20, 100}
            );
            DrawCircle(
                xOsso + comprimentoOsso, 
                yOsso + 5 + larguraOsso/2, 
                raioExtremidade + 1, 
                (Color){20, 20, 20, 100}
            );
            
            // ---- PARTE 1: EXTREMIDADES DO OSSO (forma melhorada) ----
            
            // Extremidade esquerda com gradiente para dar profundidade
            DrawCircleGradient(
                xOsso, 
                yOsso + 5 + larguraOsso/2, 
                raioExtremidade,
                corOssoPrincipal,
                corSombra
            );
            
            // Extremidade direita com gradiente
            DrawCircleGradient(
                xOsso + comprimentoOsso, 
                yOsso + 5 + larguraOsso/2, 
                raioExtremidade,
                corOssoPrincipal,
                corSombra
            );
            
            // Detalhes nas extremidades (linhas cruzadas como em ossos reais)
            Color corDetalhe = (Color){220, 220, 220, 150};
            float metadeRaio = raioExtremidade * 0.6f;
            
            // Detalhes na extremidade esquerda
            DrawLineEx(
                (Vector2){xOsso - metadeRaio, yOsso + 5 + larguraOsso/2 - metadeRaio},
                (Vector2){xOsso + metadeRaio, yOsso + 5 + larguraOsso/2 + metadeRaio},
                1.5f,
                corDetalhe
            );
            DrawLineEx(
                (Vector2){xOsso - metadeRaio, yOsso + 5 + larguraOsso/2 + metadeRaio},
                (Vector2){xOsso + metadeRaio, yOsso + 5 + larguraOsso/2 - metadeRaio},
                1.5f,
                corDetalhe
            );
            
            // Detalhes na extremidade direita
            DrawLineEx(
                (Vector2){xOsso + comprimentoOsso - metadeRaio, yOsso + 5 + larguraOsso/2 - metadeRaio},
                (Vector2){xOsso + comprimentoOsso + metadeRaio, yOsso + 5 + larguraOsso/2 + metadeRaio},
                1.5f,
                corDetalhe
            );
            DrawLineEx(
                (Vector2){xOsso + comprimentoOsso - metadeRaio, yOsso + 5 + larguraOsso/2 + metadeRaio},
                (Vector2){xOsso + comprimentoOsso + metadeRaio, yOsso + 5 + larguraOsso/2 - metadeRaio},
                1.5f,
                corDetalhe
            );
            
            // ---- PARTE 2: CORPO PRINCIPAL DO OSSO (melhorado com detalhes) ----
            
            // Corpo principal do osso com gradiente para dar profundidade
            DrawRectangleGradientV(
                xOsso, 
                yOsso + 5, 
                comprimentoOsso, 
                larguraOsso, 
                corOssoPrincipal,
                corSombra
            );
            
            // Adiciona detalhes ao corpo do osso (pequenas linhas horizontais)
            for (int j = 1; j < 4; j++) {
                float yLinha = yOsso + 5 + (larguraOsso * j / 4);
                DrawLineEx(
                    (Vector2){xOsso + 4, yLinha},
                    (Vector2){xOsso + comprimentoOsso - 4, yLinha},
                    0.8f,
                    corDetalhe
                );
            }
            
            // Adiciona brilho na parte superior para efeito 3D
            DrawRectangleGradientV(
                xOsso + 3, 
                yOsso + 6, 
                comprimentoOsso - 6, 
                larguraOsso/3, 
                corBrilho,
                corOssoInterior
            );
            
            // Efeito de movimento/vibração como em Undertale (mais sutil)
            if (GetRandomValue(0, 15) == 0) {
                DrawCircle(
                    xOsso + GetRandomValue(5, (int)comprimentoOsso - 5), 
                    yOsso + GetRandomValue(5, (int)larguraOsso) + 5, 
                    1.2f, 
                    (Color){255, 255, 255, 180}
                );
            }
        }
    }
    
    // Desenha obstáculos roxos (projéteis como em Cuphead)
    for (int i = 0; i < MAX_OBSTACULOS_ROXOS; i++) {
        if (obstaculosRoxos[i].ativo) {
            // Posição e rotação do projétil
            float angulo = tempo * 180.0f;
            float xProj = obstaculosRoxos[i].posicao.x;
            float yProj = obstaculosRoxos[i].posicao.y;
            float raioProj = 12.0f;
            
            // Cores vibrantes como em Cuphead
            Color corPrincipal = (Color){180, 50, 220, 255}; // Roxo vibrante
            Color corSecundaria = (Color){220, 100, 255, 255}; // Lilás
            Color corBrilho = (Color){255, 255, 255, 200}; // Branco para brilho
            
            // Desenha o corpo principal do projétil (bola de energia)
            DrawCircle(xProj, yProj, raioProj, corPrincipal);
            
            // Desenha o núcleo interno com brilho
            DrawCircle(xProj, yProj, raioProj * 0.7f, corSecundaria);
            DrawCircle(xProj, yProj, raioProj * 0.3f, corBrilho);
            
            // Desenha raios saindo do projétil (como em Cuphead)
            for (int j = 0; j < 8; j++) {
                float raioAngulo = j * 45.0f + angulo;
                float raioComprimento = raioProj * 1.5f;
                
                // Ponto inicial e final do raio
                float x1 = xProj + raioProj * 0.8f * cosf(raioAngulo * DEG2RAD);
                float y1 = yProj + raioProj * 0.8f * sinf(raioAngulo * DEG2RAD);
                float x2 = xProj + raioComprimento * cosf(raioAngulo * DEG2RAD);
                float y2 = yProj + raioComprimento * sinf(raioAngulo * DEG2RAD);
                
                // Desenha o raio com efeito de pulsação
                float espessura = 2.0f + sinf(tempo * 10.0f + j) * 1.0f;
                Color corRaio = (Color){
                    220, 
                    100 + (int)(50 * sinf(tempo * 5.0f)), 
                    255, 
                    180 + (int)(75 * sinf(tempo * 3.0f))
                };
                
                // Desenha linha grossa para o raio
                Vector2 v1 = {x1, y1};
                Vector2 v2 = {x2, y2};
                DrawLineEx(v1, v2, espessura, corRaio);
                
                // Adiciona uma pequena bola na ponta do raio
                DrawCircle(x2, y2, espessura, corBrilho);
            }
            
            // Adiciona rastro de partículas atrás do projétil
            for (int j = 0; j < 5; j++) {
                float distRastro = j * 5.0f;
                float alphaRastro = 200 - j * 40;
                if (alphaRastro < 0) alphaRastro = 0;
                
                DrawCircle(
                    xProj + distRastro, 
                    yProj, 
                    raioProj * (1.0f - j * 0.15f),
                    (Color){180, 50, 220, (unsigned char)alphaRastro}
                );
            }
            
            // Efeito de distorção ao redor (ondas de energia)
            if (GetRandomValue(0, 10) < 3) {
                for (int j = 0; j < 2; j++) {
                    float raioOnda = raioProj * (1.5f + j * 0.5f);
                    DrawCircleLines(
                        xProj, 
                        yProj, 
                        raioOnda, 
                        (Color){220, 100, 255, 100 - j * 30}
                    );
                }
            }
        }
    }
    
    // Desenha obstáculos amarelos (moedas/power-ups como em Cuphead)
    for (int i = 0; i < MAX_OBSTACULOS_AMARELOS; i++) {
        if (obstaculosAmarelos[i].ativo) {
            float tempo = GetTime();
            float xMoeda = obstaculosAmarelos[i].posicao.x;
            float yMoeda = obstaculosAmarelos[i].posicao.y;
            float raioMoeda = 10.0f;
            float angulo = tempo * 120.0f; // Rotação constante
            
            // Efeito de pulsação para a moeda
            float escala = 1.0f + 0.1f * sinf(tempo * 5.0f);
            raioMoeda *= escala;
            
            // Cores vibrantes douradas como em Cuphead
            Color corOuro = (Color){255, 215, 0, 255}; // Dourado
            Color corBrilho = (Color){255, 255, 200, 255}; // Amarelo claro
            
            // Desenha a moeda principal (círculo dourado)
            DrawCircle(xMoeda, yMoeda, raioMoeda, corOuro);
            
            // Desenha o brilho interno
            DrawCircle(xMoeda, yMoeda, raioMoeda * 0.8f, corBrilho);
            
            // Desenha o símbolo $ no centro da moeda
            DrawText("$", xMoeda - 5, yMoeda - 10, 20, (Color){220, 160, 0, 255});
            
            // Desenha o brilho rotativo ao redor da moeda (efeito Cuphead)
            for (int j = 0; j < 4; j++) {
                float brilhoAngulo = angulo + j * 90.0f;
                float xBrilho = xMoeda + (raioMoeda * 0.9f) * cosf(brilhoAngulo * DEG2RAD);
                float yBrilho = yMoeda + (raioMoeda * 0.9f) * sinf(brilhoAngulo * DEG2RAD);
                
                DrawCircle(xBrilho, yBrilho, 2.0f, WHITE);
            }
            
            // Desenha o efeito de brilho pulsante ao redor
            for (int j = 0; j < 2; j++) {
                float raioAura = raioMoeda * (1.2f + 0.2f * j) * (1.0f + 0.1f * sinf(tempo * 8.0f + j));
                DrawCircleLines(xMoeda, yMoeda, raioAura, (Color){255, 215, 0, 100 - j * 40});
            }
            
            // Adiciona partículas de brilho ao redor
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

// Desenha a interface do jogo com todos os elementos separados e sem sobreposição
void desenharBarra(void) {
    float tempo = GetTime();
    float vidaMax = 200.0f;
    float porcentagemVida = vidaCoracao / vidaMax;
    
    // ==========================================
    // DESENHA A FASE, PONTUAÇÃO E NOME DO BOSS NO TOPO
    // ==========================================
    char textoFase[40];
    char textoBoss[40];
    char textoPontuacao[32]; // Corrige erro de declaração
    
    // Fase
    switch (faseAtual) {
        case 1: sprintf(textoFase, "FASE 1: LEMBRANÇAS"); break;
        case 2: sprintf(textoFase, "FASE 2: PESADELOS"); break;
        case 3: sprintf(textoFase, "FASE 3: CONFRONTO FINAL"); break;
        default: sprintf(textoFase, "FASE %d", faseAtual); break;
    }
    // Pontuação
    sprintf(textoPontuacao, "Pontuação: %d", (int)pontuacao);
    // Nome do boss
    if (faseAtual >= 1 && faseAtual <= 3) {
        switch (faseAtual) {
            case 1: sprintf(textoBoss, "Boss: Memória Dolorosa"); break;
            case 2: sprintf(textoBoss, "Boss: Trauma Profundo"); break;
            case 3: sprintf(textoBoss, "Boss: Emoção Caótica"); break;
        }
    } else {
        textoBoss[0] = '\0';
    }
    // Retângulo de fundo para o nome da fase (parte superior central)
    int larguraTitulo = MeasureText(textoFase, 22) + 20;
    DrawRectangle(
        AREA_JOGO_X + (AREA_JOGO_LARGURA - larguraTitulo) / 2,
        AREA_JOGO_Y - 40,
        larguraTitulo,
        30,
        (Color){ 20, 20, 20, 180 }
    );
    
    // Texto da fase
    DrawText(
        textoFase,
        AREA_JOGO_X + (AREA_JOGO_LARGURA - MeasureText(textoFase, 22)) / 2,
        AREA_JOGO_Y - 35,
        22,
        WHITE
    );
    
    // ==========================================
    // DESENHA O HP NO CANTO INFERIOR DIREITO (VISUAL APRIMORADO)
    // ==========================================
    char textoHP[20];
    sprintf(textoHP, "HP: %d/%d", (int)vidaCoracao, (int)vidaMax);
    
    // Tamanho do retângulo para o HP
    int larguraHP = 240;
    int alturaHP = 55;
    int hpX = AREA_JOGO_X + AREA_JOGO_LARGURA - larguraHP - 10;
    int hpY = AREA_JOGO_Y + AREA_JOGO_ALTURA - 65;
    
    // Retângulo de fundo para o HP com gradiente
    Color corGradiente1, corGradiente2;
    
    if (porcentagemVida > 0.7f) {
        // Verde escuro gradiente para vida alta
        corGradiente1 = (Color){0, 60, 0, 230};
        corGradiente2 = (Color){0, 40, 0, 230};
    } else if (porcentagemVida > 0.3f) {
        // Amarelo escuro gradiente para vida média
        corGradiente1 = (Color){60, 60, 0, 230};
        corGradiente2 = (Color){40, 40, 0, 230};
    } else {
        // Vermelho escuro pulsando para vida baixa
        int pulse = 40 + (int)(20 * sinf(tempo * 8.0f));
        corGradiente1 = (Color){80, 0, 0, 230};
        corGradiente2 = (Color){pulse, 0, 0, 230};
    }
    
    // Fundo com gradiente
    DrawRectangleGradientH(hpX, hpY, larguraHP, alturaHP, corGradiente1, corGradiente2);
    
    // Borda decorativa com efeito pulsante
    Color corBorda = WHITE;
    if (porcentagemVida < 0.3f) {
        // Vermelho piscante para vida baixa
        corBorda = (Color){255, 0, 0, 100 + (int)(155 * sinf(tempo * 8.0f))};
    } else if (porcentagemVida < 0.7f) {
        // Amarelo para vida média
        corBorda = (Color){255, 255, 0, 180};
    } else {
        // Verde para vida alta
        corBorda = (Color){0, 255, 0, 150 + (int)(50 * sinf(tempo * 2.0f))};
    }
    
    DrawRectangleLines(hpX, hpY, larguraHP, alturaHP, corBorda);
    // Borda externa decorativa
    DrawRectangleLines(hpX-2, hpY-2, larguraHP+4, alturaHP+4, 
        (Color){corBorda.r, corBorda.g, corBorda.b, corBorda.a/2});
    
    // Barra de vida dentro do retângulo
    int larguraBarraInterna = larguraHP - 20;
    int alturaBarraInterna = 20;
    int barraInternaX = hpX + 10;
    int barraInternaY = hpY + alturaHP - alturaBarraInterna - 8;
    
    // Fundo da barra (cinza escuro)
    DrawRectangle(barraInternaX, barraInternaY, larguraBarraInterna, alturaBarraInterna, (Color){20, 20, 20, 255});
    
    // Barra de vida atual com gradiente baseado na porcentagem de vida
    Color corBarraInicio, corBarraFim;
    
    if (porcentagemVida > 0.7f) {
        // Verde para vida alta
        corBarraInicio = (Color){100, 255, 100, 255};
        corBarraFim = (Color){0, 200, 0, 255};
    } else if (porcentagemVida > 0.3f) {
        // Amarelo para vida média
        corBarraInicio = (Color){255, 255, 100, 255};
        corBarraFim = (Color){200, 200, 0, 255};
    } else {
        // Vermelho pulsando para vida baixa
        int pulse = 100 + (int)(155 * sinf(tempo * 8.0f));
        corBarraInicio = (Color){255, pulse, pulse, 255};
        corBarraFim = (Color){200, 0, 0, 255};
    }
    
    // Desenha a barra de vida preenchida com gradiente
    DrawRectangleGradientH(
        barraInternaX, 
        barraInternaY, 
        (int)(larguraBarraInterna * porcentagemVida), 
        alturaBarraInterna, 
        corBarraInicio,
        corBarraFim
    );
    
    // Desenha marcadores de segmento na barra de HP
    for (int i = 1; i < 10; i++) {
        float x = barraInternaX + (larguraBarraInterna / 10.0f) * i;
        
        // Linha mais clara dentro da parte preenchida da barra
        if (x <= barraInternaX + (larguraBarraInterna * porcentagemVida)) {
            DrawLine(
                x, barraInternaY, x, barraInternaY + alturaBarraInterna,
                (Color){255, 255, 255, 100}
            );
        } 
        // Linha mais escura dentro da parte vazia da barra
        else {
            DrawLine(
                x, barraInternaY, x, barraInternaY + alturaBarraInterna,
                (Color){100, 100, 100, 100}
            );
        }
    }
    
    // Desenha símbolo de coração antes do texto HP
    int tamanhoCoracao = 22;
    int coracaoX = hpX + 10;
    int coracaoY = hpY + 8;
    
    // Sombra do coração
    DrawText("♥", coracaoX + 2, coracaoY + 2, tamanhoCoracao, (Color){0, 0, 0, 150});
    
    // Coração pulsando com cor baseada na vida
    float pulsacao = 1.0f;
    if (porcentagemVida < 0.3f) {
        pulsacao = 1.0f + 0.3f * sinf(tempo * 8.0f);
        DrawText("♥", coracaoX, coracaoY, tamanhoCoracao * pulsacao, corBarraInicio);
    } else {
        pulsacao = 1.0f + 0.1f * sinf(tempo * 3.0f);
        DrawText("♥", coracaoX, coracaoY, tamanhoCoracao * pulsacao, corBarraInicio);
    }
    
    // Texto do HP com efeito de tremor
    int offsetHPX = 0;
    int offsetHPY = 0;
    if (porcentagemVida < 0.3f) {
        offsetHPX = GetRandomValue(-2, 2);
        offsetHPY = GetRandomValue(-2, 2);
    }
    
    // Sombra do texto
    DrawText(
        textoHP, 
        hpX + 40 + offsetHPX + 2, 
        hpY + 10 + offsetHPY + 2, 
        25, 
        BLACK
    );
    
    // Texto principal
    DrawText(
        textoHP, 
        hpX + 40 + offsetHPX, 
        hpY + 10 + offsetHPY, 
        25, 
        (Color){255, 200 + (int)(55 * sinf(tempo * 3.0f)), 200 + (int)(55 * sinf(tempo * 3.0f)), 255}
    );
    
    // ==========================================
    // DESENHA A PONTUAÇÃO NO CANTO SUPERIOR DIREITO
    // ==========================================
    // (Removido: já declarada no início da função)
    sprintf(textoPontuacao, "PONTUAÇÃO: %.0f", pontuacao);
    
    // Retangulo de fundo para a pontuação
    DrawRectangle(
        AREA_JOGO_X + AREA_JOGO_LARGURA - 310,
        AREA_JOGO_Y - 40,
        300,
        35,
        (Color){0, 0, 0, 200}
    );
    
    // Borda do retangulo
    DrawRectangleLines(
        AREA_JOGO_X + AREA_JOGO_LARGURA - 310,
        AREA_JOGO_Y - 40,
        300,
        35,
        (Color){255, 255, 255, 200 + (int)(55 * sinf(tempo * 2.0f))}
    );
    
    // Texto da pontuação com efeito de tremor
    int offsetPontX = GetRandomValue(-1, 1);
    int offsetPontY = GetRandomValue(-1, 1);
    
    DrawText(
        textoPontuacao, 
        AREA_JOGO_X + AREA_JOGO_LARGURA - 290 + offsetPontX, 
        AREA_JOGO_Y - 35 + offsetPontY, 
        25, 
        (Color){255, 255, 0, 255}
    );
    
    // ==========================================
    // DESENHA A BARRA DE VIDA NA PARTE INFERIOR
    // ==========================================
    
    // Configuração da barra de vida aprimorada
    int larguraBarra = 300; // Barra mais larga
    int alturaBarra = 40;   // Barra mais alta
    int barraX = AREA_JOGO_X + AREA_JOGO_LARGURA - larguraBarra - 100;
    int barraY = AREA_JOGO_Y + AREA_JOGO_ALTURA + 10;
    
    // Fundo da barra (escuro com gradiente)
    DrawRectangleGradientH(
        barraX - 5, 
        barraY - 5, 
        larguraBarra + 10, 
        alturaBarra + 10, 
        (Color){40, 0, 0, 255},
        (Color){80, 0, 0, 255}
    );
    
    // Borda da barra com efeito pulsante
    Color corBordaBarra = (Color){
        255,
        100 + (int)(55 * sinf(tempo * 2.0f)), 
        100 + (int)(55 * sinf(tempo * 2.0f)),
        255
    };
    
    // Borda interna
    DrawRectangleLines(
        barraX, 
        barraY, 
        larguraBarra, 
        alturaBarra, 
        corBordaBarra
    );
    
    // Borda externa
    DrawRectangleLines(
        barraX - 5, 
        barraY - 5, 
        larguraBarra + 10, 
        alturaBarra + 10, 
        (Color){255, 255, 255, 100 + (int)(55 * sinf(tempo * 3.0f))}
    );
    
    // Fundo da barra (vermelho escuro)
    DrawRectangle(
        barraX, 
        barraY, 
        larguraBarra, 
        alturaBarra, 
        (Color){60, 0, 0, 255}
    );
    
    // Barra de vida atual com cor baseada na porcentagem
    Color corVida;
    if (porcentagemVida > 0.7f) {
        // Verde para HP alto
        corVida = (Color){
            100 + (int)(55 * sinf(tempo * 3.0f)), 
            255, 
            100 + (int)(55 * sinf(tempo * 3.0f)), 
            255
        };
    } else if (porcentagemVida > 0.3f) {
        // Amarelo para HP médio
        corVida = (Color){
            255, 
            255, 
            0, 
            255
        };
    } else {
        // Vermelho pulsante para HP baixo
        corVida = (Color){
            255, 
            100 + (int)(155 * sinf(tempo * 8.0f)), 
            100 + (int)(155 * sinf(tempo * 8.0f)), 
            255
        };
    }
    
    // Desenha a barra de vida com gradiente
    DrawRectangleGradientH(
        barraX, 
        barraY, 
        (int)(larguraBarra * porcentagemVida), 
        alturaBarra, 
        corVida,
        (Color){corVida.r, corVida.g/2, corVida.b/2, corVida.a}
    );
    
    // Desenha linhas de segmento na barra (visual de "blocos" de vida)
    for (int i = 1; i < 10; i++) {
        int x = barraX + (larguraBarra / 10) * i;
        if (x < barraX + (int)(larguraBarra * porcentagemVida)) {
            DrawLine(
                x, 
                barraY, 
                x, 
                barraY + alturaBarra, 
                (Color){255, 255, 255, 80}
            );
        }
    }
    
    // Texto "HP" com efeito de tremor ao lado da barra
    int offsetHPBarraX = 0;
    int offsetHPBarraY = 0;
    
    if (porcentagemVida < 0.3f) {
        offsetHPBarraX = GetRandomValue(-2, 2);
        offsetHPBarraY = GetRandomValue(-2, 2);
    }
    
    // Sombra do texto "HP"
    DrawText("HP", barraX - 50 + offsetHPBarraX + 2, barraY + 5 + offsetHPBarraY + 2, 30, BLACK);
    // Texto "HP" principal
    DrawText("HP", barraX - 50 + offsetHPBarraX, barraY + 5 + offsetHPBarraY, 30, (Color){255, 0, 0, 255});
    
    // Valor numérico da vida com efeito de pulsação
    char textoVida[20];
    sprintf(textoVida, "%d / %d", (int)vidaCoracao, (int)vidaMax);
    
    // Efeito de pulsação para o texto
    float pulsacaoTexto = 1.0f;
    if (porcentagemVida < 0.3f) {
        pulsacaoTexto = 1.0f + 0.3f * sinf(tempo * 8.0f);
    } else {
        pulsacaoTexto = 1.0f + 0.1f * sinf(tempo * 3.0f);
    }
    int tamanhoTexto = 25 + (int)(5 * pulsacaoTexto);
    
    // Sombra do texto
    DrawText(
        textoVida, 
        barraX + larguraBarra + 12, 
        barraY + 7, 
        tamanhoTexto, 
        BLACK
    );
    
    // Texto principal com cor baseada na porcentagem de vida
    DrawText(
        textoVida, 
        barraX + larguraBarra + 10, 
        barraY + 5, 
        tamanhoTexto, 
        corVida
    );
    
    // Desenha os fragmentos (pontuação) no canto inferior esquerdo
    char textoFragmentos[30];
    sprintf(textoFragmentos, "Fragmentos: %.0f", pontuacao);
    
    // Retangulo de fundo para os fragmentos
    DrawRectangle(
        AREA_JOGO_X + 10,
        barraY,
        250,
        35,
        (Color){0, 0, 0, 200}
    );
    
    // Borda do retangulo
    DrawRectangleLines(
        AREA_JOGO_X + 10,
        barraY,
        250,
        35,
        (Color){255, 255, 255, 200 + (int)(55 * sinf(tempo * 2.0f))}
    );
    
    // Texto dos fragmentos com efeito de tremor
    int offsetFragX = GetRandomValue(-1, 1);
    int offsetFragY = GetRandomValue(-1, 1);
    
    DrawText(
        textoFragmentos, 
        AREA_JOGO_X + 30 + offsetFragX, 
        barraY + 5 + offsetFragY, 
        25, 
        (Color){255, 255, 0, 255}
    );
    
    // Barra de vida com gradiente
    for (int i = 0; i < (int)vidaCoracao; i++) {
        float fator = (float)i / 200.0f; // 0.0 a 1.0
        Color corGradiente = (Color){
            (unsigned char)(corVida.r * (0.7f + 0.3f * fator)),
            (unsigned char)(corVida.g * fator),
            (unsigned char)(corVida.b * (1.0f - fator * 0.5f)),
            255
        };
        
        DrawRectangle(barraX + i, AREA_JOGO_Y + AREA_JOGO_ALTURA + 10, 1, 20, corGradiente);
    }
    
    // Adiciona efeito de brilho na borda da barra
    DrawRectangleLines(
        barraX, 
        AREA_JOGO_Y + AREA_JOGO_ALTURA + 10, 
        200, 
        20, 
        WHITE
    );
    
    // Desenha pequenos corações na barra de vida
    for (int i = 0; i < 5; i++) {
        if (vidaCoracao >= (i+1) * 40) {
            DrawText("♥", 
                     barraX + 10 + i * 40, 
                     AREA_JOGO_Y + AREA_JOGO_ALTURA + 13, 
                     15, 
                     WHITE);
        }
    }
    
    // Adiciona mensagens motivacionais baseadas na vida
    const char* mensagem;
    if (vidaCoracao < 50) {
        mensagem = "Não desista! A luz ainda pode renascer...";
    } else if (vidaCoracao < 100) {
        mensagem = "Continue lutando! Seus sentimentos estão retornando...";
    } else if (vidaCoracao < 150) {
        mensagem = "Você está recuperando sua essência!";
    } else {
        mensagem = "Seu coração pulsa com força renovada!";
    }
    
    // Desenha a mensagem motivacional na parte inferior da tela
    DrawRectangle(
        AREA_JOGO_X + (AREA_JOGO_LARGURA - MeasureText(mensagem, 18) - 40) / 2,
        AREA_JOGO_Y + AREA_JOGO_ALTURA + 55,
        MeasureText(mensagem, 18) + 40,
        30,
        (Color){0, 0, 0, 150}
    );
    
    DrawText(mensagem, 
             AREA_JOGO_X + AREA_JOGO_LARGURA/2 - MeasureText(mensagem, 18)/2, 
             AREA_JOGO_Y + AREA_JOGO_ALTURA + 60, 
             18, 
             (Color){220, 220, 220, 255});
}

// Desenha o boss com efeitos assustadores
void desenharBoss(void) {
    static float tempoAnimacao = 0.0f;
    static float posY = 0.0f;
    static float posX = 0.0f;
    static float escala = 1.0f;
    static int frameAtaque = 0;
    static bool atacando = false;
    
    tempoAnimacao += GetFrameTime();
    
    if (faseAtual == 2 || faseAtual == 3) {
        // Posição base do boss
        if (posY == 0.0f) {
            posY = AREA_JOGO_Y + (AREA_JOGO_ALTURA - texturaBoss.height) / 2;
        }
        
        if (posX == 0.0f) {
            posX = AREA_JOGO_X + AREA_JOGO_LARGURA - texturaBoss.width - 20;
        }
        
        // Movimento flutuante vertical
        posY += sinf(tempoAnimacao * 2.0f) * 0.5f;
        
        // Pulsação
        escala = 1.0f + sinf(tempoAnimacao * 3.0f) * 0.05f;
        
        // Comportamento de ataque (a cada 3-5 segundos)
        if (!atacando && GetRandomValue(0, 180) == 0) {
            atacando = true;
            frameAtaque = 0;
        }
        
        if (atacando) {
            frameAtaque++;
            
            if (frameAtaque < 30) {
                // Preparação para o ataque (recua)
                posX += 0.5f;
                escala *= 0.99f;
            } else if (frameAtaque < 40) {
                // Ataque rápido em direção ao jogador
                posX -= 5.0f;
                escala *= 1.05f;
                
                // Limita o movimento para não sair da área de jogo
                if (posX < AREA_JOGO_X + AREA_JOGO_LARGURA / 2) {
                    posX = AREA_JOGO_X + AREA_JOGO_LARGURA / 2;
                }
                
                // Verifica colisão com o jogador durante o ataque
                Rectangle bossRect = (Rectangle){posX, posY, texturaBoss.width * escala, texturaBoss.height * escala};
                Rectangle coracaoRect = (Rectangle){posicaoCoracao.x, posicaoCoracao.y, TAMANHO_CORACAO, TAMANHO_CORACAO};
                
                if (CheckCollisionRecs(bossRect, coracaoRect)) {
                    // Dano severo ao jogador se atingido pelo ataque do boss
                    vidaCoracao -= 40.0f;
                    // Efeito visual de dano
                    for (int i = 0; i < 20; i++) {
                        // Código para efeito visual aqui
                    }
                }
            } else if (frameAtaque < 80) {
                // Retorno à posição original
                posX += 1.0f;
                if (posX > AREA_JOGO_X + AREA_JOGO_LARGURA - texturaBoss.width - 20) {
                    posX = AREA_JOGO_X + AREA_JOGO_LARGURA - texturaBoss.width - 20;
                }
            } else {
                atacando = false;
            }
        }
        
        // Desenha o boss com efeitos
        if (faseAtual == 2) {
            // Efeito de aura para o boss da fase 2
            for (int i = 0; i < 3; i++) {
                float tamanhoAura = (float)(i + 1) * 5.0f;
                Color corAura = (Color){75, 0, 130, 100 - i * 30};
                DrawTextureEx(texturaBoss, 
                             (Vector2){posX - tamanhoAura/2, posY - tamanhoAura/2}, 
                             0.0f, 
                             escala + tamanhoAura/100.0f, 
                             corAura);
            }
            
            // Texto assustador piscando
            if ((int)(tempoAnimacao * 2) % 2 == 0) {
                DrawText("MEDO", posX + 20, posY - 20, 20, RED);
            } else {
                DrawText("PESADELO", posX + 10, posY + texturaBoss.height + 5, 20, PURPLE);
            }
        } else if (faseAtual == 3) {
            // Efeito de sangue para o boss da fase 3
            for (int i = 0; i < 5; i++) {
                DrawTextureEx(texturaBoss, 
                             (Vector2){posX + GetRandomValue(-3, 3), posY + GetRandomValue(-3, 3)}, 
                             0.0f, 
                             escala, 
                             (Color){255, 0, 0, 50});
            }
            
            // Gotas de sangue caindo
            for (int i = 0; i < 3; i++) {
                float gotaX = posX + GetRandomValue(0, (int)texturaBoss.width);
                float gotaY = posY + texturaBoss.height + (sinf(tempoAnimacao * 5 + i) + 1) * 20;
                DrawCircle(gotaX, gotaY, 3, RED);
            }
            
            // Texto assustador piscando rapidamente
            if ((int)(tempoAnimacao * 5) % 2 == 0) {
                DrawText("MORTE", posX + 30, posY - 30, 25, RED);
            } else if ((int)(tempoAnimacao * 5) % 3 == 0) {
                DrawText("DESESPERO", posX + 10, posY + texturaBoss.height + 10, 25, RED);
            } else {
                DrawText("AGONIA", posX + 20, posY + texturaBoss.height/2, 25, RED);
            }
        }
        
        // Desenha o boss principal
        DrawTextureEx(texturaBoss, (Vector2){posX, posY}, 0.0f, escala, WHITE);
        
        // Efeito de distorção (glitch) ocasional
        if (GetRandomValue(0, 30) == 0) {
            int numGlitches = GetRandomValue(3, 8);
            for (int i = 0; i < numGlitches; i++) {
                int glitchY = posY + GetRandomValue(0, (int)texturaBoss.height);
                int glitchAltura = GetRandomValue(2, 8);
                int glitchDeslocamento = GetRandomValue(-10, 10);
                
                Rectangle srcRec = (Rectangle){0, glitchY - posY, texturaBoss.width, glitchAltura};
                Rectangle destRec = (Rectangle){posX + glitchDeslocamento, glitchY, texturaBoss.width * escala, glitchAltura};
                
                DrawTexturePro(texturaBoss, srcRec, destRec, (Vector2){0, 0}, 0.0f, WHITE);
            }
        }
        
        // Texto do boss
        char textoBoss[50] = "";
        if (faseAtual == 2) {
            strcpy(textoBoss, "ECOS DO PASSADO");
        } else if (faseAtual == 3) {
            strcpy(textoBoss, "GUARDIAO DAS MEMORIAS");
        }
        
        if (strlen(textoBoss) > 0) {
            int larguraTexto = MeasureText(textoBoss, 20);
            DrawText(
                textoBoss, 
                posX + texturaBoss.width/2 - larguraTexto/2, 
                posY - 30, 
                20, 
                RED
            );
        }
    }
}

// Reinicia o jogo para um novo começo
void reiniciarJogo(void) {
    // Reinicia as variáveis do jogo
    posicaoCoracao.x = AREA_JOGO_X + 50;
    posicaoCoracao.y = AREA_JOGO_Y + AREA_JOGO_ALTURA / 2;
    vidaCoracao = 200.0f;
    pontuacao = 0;
    faseAtual = 1;
    jogadorMovendo = false;
    contadorObstaculos = 0;
    velocidadeBase = 2.5f;
    velocidadeCoracao = VELOCIDADE_CORACAO;
    modoChefao = false;
    
    // Reinicia obstáculos
    inicializarObstaculos();
    
    // Reinicia bosses
    inicializarBosses();
    
    printf("Jogo reiniciado!\n");
}

// Desenha a fase atual com efeitos visuais
void desenharFase(void) {
    char textoFase[50];
    
    // Texto diferente para cada fase
    switch(faseAtual) {
        case 1:
            sprintf(textoFase, "FASE 1: LEMBRANÇAS");
            break;
        case 2:
            sprintf(textoFase, "FASE 2: PESADELOS");
            break;
        case 3:
            sprintf(textoFase, "FASE 3: CONFRONTO FINAL");
            break;
        default:
            sprintf(textoFase, "FASE %d", faseAtual);
    }
    
    // Desenha a borda da área de jogo
    Color corBorda;
    switch(faseAtual) {
        case 1: corBorda = (Color){100, 100, 255, 200}; break; // Azul
        case 2: corBorda = (Color){150, 0, 200, 200}; break;   // Roxo
        case 3: corBorda = (Color){200, 0, 100, 200}; break;   // Vermelho
        default: corBorda = LIGHTGRAY;
    }
    
    // Desenha a borda
    DrawRectangleLines(AREA_JOGO_X, AREA_JOGO_Y, AREA_JOGO_LARGURA, AREA_JOGO_ALTURA, corBorda);
    
    // Sombra para o texto
    DrawText(textoFase, AREA_JOGO_X + 2, AREA_JOGO_Y - 28, 20, BLACK);
    // Texto principal
    DrawText(textoFase, AREA_JOGO_X, AREA_JOGO_Y - 30, 20, corBorda);
}

// Finaliza o jogo e libera recursos
void finalizarJogo(void) {
    // Descarrega texturas
    UnloadTexture(texturaCoracao);
    if (faseAtual >= 2) {
        UnloadTexture(texturaBoss);
    }
}

// Initialize damage numbers
void inicializarNumerosDano(void) {
    for (int i = 0; i < MAX_NUMEROS_DANO; i++) {
        numerosDano[i].ativo = false;
    }
}

// Add a damage number
void adicionarNumeroDano(float valor, Vector2 posicao, bool ehDano) {
    // Find an inactive slot
    for (int i = 0; i < MAX_NUMEROS_DANO; i++) {
        if (!numerosDano[i].ativo) {
            numerosDano[i].posicao = posicao;
            numerosDano[i].valor = valor;
            numerosDano[i].tempo = 1.0f;  // 1 second lifetime
            numerosDano[i].ativo = true;
            numerosDano[i].ehDano = ehDano;
            
            // Set initial velocity (moves up)
            numerosDano[i].velocidade.x = GetRandomValue(-50, 50) / 100.0f;
            numerosDano[i].velocidade.y = -2.5f;
            
            // Set color based on damage or healing
            if (ehDano) {
                numerosDano[i].cor = (Color){255, 0, 0, 255};  // Red for damage
            } else {
                numerosDano[i].cor = (Color){0, 255, 0, 255};  // Green for healing
            }
            
            break;
        }
    }
}

// Update damage numbers
void atualizarNumerosDano(void) {
    for (int i = 0; i < MAX_NUMEROS_DANO; i++) {
        if (numerosDano[i].ativo) {
            // Update position
            numerosDano[i].posicao.x += numerosDano[i].velocidade.x;
            numerosDano[i].posicao.y += numerosDano[i].velocidade.y;
            
            // Slow down movement
            numerosDano[i].velocidade.y *= 0.95f;
            
            // Update lifetime
            numerosDano[i].tempo -= GetFrameTime();
            
            // Deactivate when time runs out
            if (numerosDano[i].tempo <= 0) {
                numerosDano[i].ativo = false;
            }
        }
    }
}

// Draw damage numbers
void desenharNumerosDano(void) {
    for (int i = 0; i < MAX_NUMEROS_DANO; i++) {
        if (numerosDano[i].ativo) {
            // Calculate alpha based on remaining time
            int alpha = (int)(255 * numerosDano[i].tempo);
            if (alpha < 0) alpha = 0;
            if (alpha > 255) alpha = 255;
            
            // Set color with appropriate alpha
            Color color = numerosDano[i].cor;
            color.a = alpha;
            
            // Calculate size based on damage value and remaining time
            float size = 20.0f;
            if (numerosDano[i].valor > 30.0f) size = 30.0f; 
            if (numerosDano[i].valor > 50.0f) size = 35.0f;
            
            // Add a size pulsation effect
            float pulseFactor = 1.0f + 0.2f * sinf(GetTime() * 10.0f);
            size *= pulseFactor;
            
            // Format text based on whether it's damage or healing
            char texto[20];
            if (numerosDano[i].ehDano) {
                sprintf(texto, "-%d", (int)numerosDano[i].valor);
            } else {
                sprintf(texto, "+%d", (int)numerosDano[i].valor);
            }
            
            // Draw text shadow for better visibility
            DrawText(texto, numerosDano[i].posicao.x + 2, numerosDano[i].posicao.y + 2, size, BLACK);
            DrawText(texto, numerosDano[i].posicao.x, numerosDano[i].posicao.y, size, color);
        }
    }
}
