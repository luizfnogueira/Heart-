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
    
    // Verifica se o jogador perdeu
    if (vidaCoracao <= 0) {
        // Game Over
        // PlaySound(somGameOver);
        reiniciarJogo();
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
                // Dano aumentado
                vidaCoracao -= 15.0f + faseAtual * 5.0f;
                
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
                // Dano severo
                vidaCoracao -= 25.0f + faseAtual * 7.0f;
                
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
                // Recupera menos vida, tornando mais difícil se manter vivo
                vidaCoracao += 10.0f;
                if (vidaCoracao > 200.0f) vidaCoracao = 200.0f;
                
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
    
    // Desenha informações da fase atual
    {
        char textoFase[50];
        switch(faseAtual) {
            case 1: sprintf(textoFase, "FASE 1: LEMBRANÇAS"); break;
            case 2: sprintf(textoFase, "FASE 2: PESADELOS"); break;
            case 3: sprintf(textoFase, "FASE 3: CONFRONTO FINAL"); break;
            default: sprintf(textoFase, "FASE %d", faseAtual); break;
        }
        DrawText(textoFase, AREA_JOGO_X + 2, AREA_JOGO_Y - 28, 20, BLACK); // Sombra
        DrawText(textoFase, AREA_JOGO_X, AREA_JOGO_Y - 30, 20, WHITE);      // Texto principal
    }
    
    // Desenha a pontuação e a vida
    DrawText(TextFormat("Pontuação: %d", (int)pontuacao), AREA_JOGO_X, 10, 20, WHITE);
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
    
    // Desenha obstáculos brancos (ossos como em Undertale)
    for (int i = 0; i < MAX_OBSTACULOS_BRANCOS; i++) {
        if (obstaculosBrancos[i].ativo) {
            // Posição e dimensões do osso
            float comprimentoOsso = 30.0f;
            float larguraOsso = 10.0f;
            float xOsso = obstaculosBrancos[i].posicao.x;
            float yOsso = obstaculosBrancos[i].posicao.y;
            
            // Desenha o corpo do osso (retângulo branco)
            DrawRectangle(
                xOsso, 
                yOsso + 5, 
                comprimentoOsso, 
                larguraOsso, 
                WHITE
            );
            
            // Desenha as extremidades do osso (círculos brancos nas pontas)
            // Extremidade esquerda
            DrawCircle(
                xOsso, 
                yOsso + 5 + larguraOsso/2, 
                larguraOsso/2 + 2, 
                WHITE
            );
            
            // Extremidade direita
            DrawCircle(
                xOsso + comprimentoOsso, 
                yOsso + 5 + larguraOsso/2, 
                larguraOsso/2 + 2, 
                WHITE
            );
            
            // Adiciona sombra para dar profundidade
            DrawRectangle(
                xOsso + 2, 
                yOsso + 7, 
                comprimentoOsso - 4, 
                larguraOsso - 4, 
                (Color){220, 220, 220, 255}
            );
            
            // Adiciona brilho para dar efeito 3D
            DrawRectangle(
                xOsso + 3, 
                yOsso + 8, 
                comprimentoOsso - 6, 
                2, 
                (Color){255, 255, 255, 200}
            );
            
            // Efeito de tremor como em Undertale (movimento sutil)
            if (GetRandomValue(0, 20) == 0) {
                DrawCircle(
                    xOsso + GetRandomValue(0, (int)comprimentoOsso), 
                    yOsso + GetRandomValue(0, (int)larguraOsso) + 5, 
                    1, 
                    (Color){255, 255, 255, 150}
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
    // Retângulo de fundo maior para comportar os três textos
    DrawRectangle(
        AREA_JOGO_X + (AREA_JOGO_LARGURA - 340) / 2,
        AREA_JOGO_Y - 90,
        340,
        70,
        (Color){ 20, 20, 20, 180 }
    );
    // Texto da fase
    DrawText(
        textoFase,
        AREA_JOGO_X + (AREA_JOGO_LARGURA - MeasureText(textoFase, 22)) / 2,
        AREA_JOGO_Y - 85,
        22,
        WHITE
    );
    // Texto da pontuação
    DrawText(
        textoPontuacao, 
        AREA_JOGO_X + (AREA_JOGO_LARGURA - MeasureText(textoPontuacao, 22)) / 2, 
        AREA_JOGO_Y - 60, 
        22, 
        (Color){255, 255, 0, 255}
    );
    // Texto do boss
    DrawText(
        textoBoss, 
        AREA_JOGO_X + (AREA_JOGO_LARGURA - MeasureText(textoBoss, 22)) / 2, 
        AREA_JOGO_Y - 35, 
        22, 
        (Color){255, 100, 100, 255}
    );
    
    // ==========================================
    // DESENHA O HP NO CANTO SUPERIOR ESQUERDO
    // ==========================================
    char textoHP[20];
    sprintf(textoHP, "HP: %d/%d", (int)vidaCoracao, (int)vidaMax);
    
    // Retangulo de fundo para o HP
    DrawRectangle(
        AREA_JOGO_X + 10,
        AREA_JOGO_Y - 40,
        180,
        35,
        (Color){0, 0, 0, 200}
    );
    
    // Borda do retangulo
    DrawRectangleLines(
        AREA_JOGO_X + 10,
        AREA_JOGO_Y - 40,
        180,
        35,
        (Color){255, 255, 255, 200 + (int)(55 * sinf(tempo * 2.0f))}
    );
    
    // Texto do HP com efeito de tremor
    int offsetHPX = GetRandomValue(-1, 1);
    int offsetHPY = GetRandomValue(-1, 1);
    
    DrawText(
        textoHP, 
        AREA_JOGO_X + 30 + offsetHPX, 
        AREA_JOGO_Y - 35 + offsetHPY, 
        25, 
        (Color){255, 0, 0, 255}
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
    
    // Configuração da barra de vida
    int larguraBarra = 250; // Barra mais larga
    int alturaBarra = 35;   // Barra mais alta
    int barraX = AREA_JOGO_X + 100;
    int barraY = AREA_JOGO_Y + AREA_JOGO_ALTURA + 10;
    
    // Borda da barra
    DrawRectangleLines(
        barraX, 
        barraY, 
        larguraBarra, 
        alturaBarra, 
        WHITE
    );
    
    // Fundo da barra (vermelho escuro)
    DrawRectangle(
        barraX, 
        barraY, 
        larguraBarra, 
        alturaBarra, 
        (Color){100, 0, 0, 255}
    );
    
    // Barra de vida atual (amarelo/laranja com efeito pulsante)
    Color corVida = (Color){
        255, 
        200 + (int)(55 * sinf(tempo * 3.0f)), 
        0, 
        255
    };
    
    DrawRectangle(
        barraX, 
        barraY, 
        (int)(larguraBarra * porcentagemVida), 
        alturaBarra, 
        corVida
    );
    
    // Texto "HP" com efeito de tremor ao lado da barra
    int offsetHPBarraX = GetRandomValue(-2, 2);
    int offsetHPBarraY = GetRandomValue(-2, 2);
    DrawText("HP", barraX - 50 + offsetHPBarraX, barraY + 5 + offsetHPBarraY, 30, (Color){255, 0, 0, 255});
    
    // Valor numérico da vida com efeito de pulsação
    char textoVida[20];
    sprintf(textoVida, "%d / %d", (int)vidaCoracao, (int)vidaMax);
    
    // Efeito de pulsação para o texto
    float pulsacao = 1.0f + 0.2f * sinf(tempo * 4.0f);
    int tamanhoTexto = 25 + (int)(5 * pulsacao);
    
    // Sombra do texto
    DrawText(
        textoVida, 
        barraX + larguraBarra + 12, 
        barraY + 7, 
        tamanhoTexto, 
        BLACK
    );
    
    // Texto principal com efeito de cor pulsante
    DrawText(
        textoVida, 
        barraX + larguraBarra + 10, 
        barraY + 5, 
        tamanhoTexto, 
        (Color){255, 200 + (int)(55 * sinf(tempo * 3.0f)), 0, 255}
    );
    
    // Desenha os fragmentos (pontuação) no canto inferior direito
    char textoFragmentos[30];
    sprintf(textoFragmentos, "Fragmentos: %.0f", pontuacao);
    
    // Retangulo de fundo para os fragmentos
    DrawRectangle(
        barraX + larguraBarra + 150,
        barraY,
        250,
        35,
        (Color){0, 0, 0, 200}
    );
    
    // Borda do retangulo
    DrawRectangleLines(
        barraX + larguraBarra + 150,
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
        barraX + larguraBarra + 170 + offsetFragX, 
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
        
        DrawRectangle(AREA_JOGO_X + 100 + i, AREA_JOGO_Y + AREA_JOGO_ALTURA + 10, 1, 20, corGradiente);
    }
    
    // Adiciona efeito de brilho na borda da barra
    DrawRectangleLines(
        AREA_JOGO_X + 100, 
        AREA_JOGO_Y + AREA_JOGO_ALTURA + 10, 
        200, 
        20, 
        WHITE
    );
    
    // Desenha pequenos corações na barra de vida
    for (int i = 0; i < 5; i++) {
        if (vidaCoracao >= (i+1) * 40) {
            DrawText("♥", 
                     AREA_JOGO_X + 110 + i * 40, 
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
