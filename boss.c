#include "jogo.h"
#include <stdio.h>
#include <raylib.h>
#include <raymath.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

// Nota: A função Clamp já é fornecida pela biblioteca raylib

// Declarações de variáveis externas
extern float vidaCoracao;
extern Vector2 posicaoCoracao;
extern float pontuacao;
extern int faseAtual;
extern NivelDificuldade dificuldadeAtual;

// Variáveis para efeitos visuais de dano
extern float efeitoDanoTempo;
extern float ultimoDano;

// Array de bosses
Boss bosses[MAX_BOSSES];

// Nomes e descrições dos bosses
const char* nomesBosses[] = {
    "Memória Dolorosa",
    "Trauma Profundo",
    "Emoção Caótica"
};

const char* descricoesBosses[] = {
    "Uma lembrança que se recusa a ser esquecida, atacando com padrões repetitivos e implacáveis.",
    "Um trauma que se manifesta como uma entidade sombria, perseguindo o coração com ataques calculados.",
    "A personificação do caos emocional, com ataques imprevisíveis e devastadores."
};

// Inicializa os bosses
void inicializarBosses(void) {
    for (int i = 0; i < MAX_BOSSES; i++) {
        bosses[i].posicao = (Vector2){ AREA_JOGO_X + AREA_JOGO_LARGURA / 2, AREA_JOGO_Y + 100 };
        bosses[i].vida = 500.0f * (i + 1);
        bosses[i].vidaMaxima = bosses[i].vida;
        bosses[i].fase = i + 1;
        bosses[i].ativo = false;
        bosses[i].tempo = 0.0f;
        bosses[i].tempoAtaque = 0.0f;
        bosses[i].tempoEntreAtaques = 2.0f - (i * 0.3f); // Bosses mais avançados atacam mais rápido
        bosses[i].padraoAtual = GetRandomValue(0, MAX_PADROES_ATAQUE - 1);
        
        // Criar textura de boss em estilo pixel art assustador
        Image img = GenImageColor(64, 64, (Color){ 0, 0, 0, 0 }); // Imagem transparente como base
        
        // Cores para os diferentes bosses
        Color corPrincipal;
        Color corSecundaria;
        Color corOlhos;
        
        switch (i) {
            case 0: // Primeiro boss - Vermelho/Preto (Medo)
                corPrincipal = (Color){ 180, 20, 20, 255 }; // Vermelho escuro
                corSecundaria = (Color){ 40, 0, 0, 255 }; // Quase preto avermelhado
                corOlhos = (Color){ 255, 255, 0, 255 }; // Olhos amarelos
                break;
            case 1: // Segundo boss - Roxo/Preto (Ansiedade)
                corPrincipal = (Color){ 130, 20, 180, 255 }; // Roxo
                corSecundaria = (Color){ 40, 0, 60, 255 }; // Roxo escuro
                corOlhos = (Color){ 0, 255, 255, 255 }; // Olhos ciano
                break;
            case 2: // Terceiro boss - Preto/Vermelho (Desespero)
                corPrincipal = (Color){ 30, 30, 30, 255 }; // Quase preto
                corSecundaria = (Color){ 200, 0, 0, 255 }; // Vermelho vivo
                corOlhos = (Color){ 255, 0, 0, 255 }; // Olhos vermelhos
                break;
        }
        
        // Desenhar o corpo principal do boss (forma base)
        for (int y = 10; y < 54; y++) {
            for (int x = 10; x < 54; x++) {
                // Criar forma irregular para o corpo
                if ((x + y) % 7 != 0 && GetRandomValue(0, 10) > 1) {
                    ImageDrawPixel(&img, x, y, corPrincipal);
                }
            }
        }
        
        // Adicionar detalhes assustadores
        // Olhos
        for (int ey = 0; ey < 8; ey++) {
            for (int ex = 0; ex < 8; ex++) {
                // Olho esquerdo
                if ((ex-4)*(ex-4) + (ey-4)*(ey-4) < 16) {
                    ImageDrawPixel(&img, 20 + ex, 25 + ey, corOlhos);
                }
                // Olho direito
                if ((ex-4)*(ex-4) + (ey-4)*(ey-4) < 16) {
                    ImageDrawPixel(&img, 36 + ex, 25 + ey, corOlhos);
                }
            }
        }
        
        // Pupilas (que seguem o jogador)
        ImageDrawPixel(&img, 22, 27, BLACK);
        ImageDrawPixel(&img, 23, 27, BLACK);
        ImageDrawPixel(&img, 22, 28, BLACK);
        ImageDrawPixel(&img, 23, 28, BLACK);
        
        ImageDrawPixel(&img, 38, 27, BLACK);
        ImageDrawPixel(&img, 39, 27, BLACK);
        ImageDrawPixel(&img, 38, 28, BLACK);
        ImageDrawPixel(&img, 39, 28, BLACK);
        
        // Boca assustadora (diferente para cada boss)
        if (i == 0) { // Primeiro boss - boca dentada
            for (int mx = 20; mx < 44; mx++) {
                // Linha da boca
                ImageDrawPixel(&img, mx, 40, BLACK);
                // Dentes
                if (mx % 4 == 0) {
                    ImageDrawPixel(&img, mx, 39, WHITE);
                    ImageDrawPixel(&img, mx+1, 39, WHITE);
                    ImageDrawPixel(&img, mx, 41, WHITE);
                    ImageDrawPixel(&img, mx+1, 41, WHITE);
                }
            }
        } else if (i == 1) { // Segundo boss - boca em zigue-zague
            for (int mx = 20; mx < 44; mx++) {
                int offset = (mx % 4 == 0 || mx % 4 == 1) ? 2 : -2;
                ImageDrawPixel(&img, mx, 40 + offset, corSecundaria);
            }
        } else { // Terceiro boss - boca com sangue
            for (int mx = 20; mx < 44; mx++) {
                ImageDrawPixel(&img, mx, 40, (Color){255, 0, 0, 255});
                if (GetRandomValue(0, 2) == 0) {
                    ImageDrawPixel(&img, mx, 41, (Color){255, 0, 0, 255});
                    if (GetRandomValue(0, 3) == 0) {
                        ImageDrawPixel(&img, mx, 42, (Color){255, 0, 0, 255});
                    }
                }
            }
        }
        
        // Adicionar detalhes extras aleatórios para parecer mais assustador
        for (int d = 0; d < 50; d++) {
            int px = GetRandomValue(5, 59);
            int py = GetRandomValue(5, 59);
            ImageDrawPixel(&img, px, py, corSecundaria);
        }
        
        // Adicionar aura ao redor do boss
        for (int y = 2; y < 62; y++) {
            for (int x = 2; x < 62; x++) {
                if (x < 5 || x > 59 || y < 5 || y > 59) {
                    if (GetRandomValue(0, 2) == 0) {
                        Color corAura = corSecundaria;
                        corAura.a = 100; // Semi-transparente
                        ImageDrawPixel(&img, x, y, corAura);
                    }
                }
            }
        }
        
        bosses[i].textura = LoadTextureFromImage(img);
        UnloadImage(img);
        
        bosses[i].frameAtual = (Rectangle){ 0, 0, 64, 64 };
        bosses[i].numFrames = 1;
        bosses[i].tempoFrame = 0.0f;
        
        strcpy(bosses[i].nome, nomesBosses[i]);
        strcpy(bosses[i].descricao, descricoesBosses[i]);
        
        // Inicializa os projéteis
        for (int j = 0; j < MAX_PROJETEIS; j++) {
            bosses[i].projeteis[j].ativo = false;
        }
        
        // Define se o boss é controlado por IA (todos são, neste caso)
        bosses[i].inteligente = true;
        strcpy(bosses[i].ultimaResposta, "Aguardando primeira decisão da IA...");
    }
    
    printf("Bosses inicializados com sucesso!\n");
}

// Atualiza os bosses
void atualizarBosses(void) {
    for (int i = 0; i < MAX_BOSSES; i++) {
        if (bosses[i].ativo) {
            bosses[i].tempo += GetFrameTime();
            bosses[i].tempoFrame += GetFrameTime();
            
            // Animação do boss (se houver múltiplos frames)
            if (bosses[i].tempoFrame >= 0.2f && bosses[i].numFrames > 1) {
                bosses[i].tempoFrame = 0.0f;
                bosses[i].frameAtual.x += bosses[i].frameAtual.width;
                if (bosses[i].frameAtual.x >= bosses[i].textura.width) {
                    bosses[i].frameAtual.x = 0;
                }
            }
            
            // Movimento do boss (padrão de movimento em 8)
            float t = bosses[i].tempo;
            float amplitude = 100.0f;
            float frequencia = 0.5f;
            
            bosses[i].posicao.x = AREA_JOGO_X + AREA_JOGO_LARGURA / 2 + amplitude * sinf(t * frequencia);
            bosses[i].posicao.y = AREA_JOGO_Y + 150 + amplitude * 0.5f * sinf(t * frequencia * 2);
            
            // Verifica se é hora de atacar
            if (bosses[i].tempo - bosses[i].tempoAtaque >= bosses[i].tempoEntreAtaques) {
                bosses[i].tempoAtaque = bosses[i].tempo;
                
                // Se o boss é controlado por IA, consulta a API para decidir o próximo ataque
                if (bosses[i].inteligente && GetRandomValue(0, 100) < 30) { // 30% de chance de consultar a IA
                    determinarProximoAtaqueBoss(&bosses[i]);
                } else {
                    // Caso contrário, escolhe um padrão aleatório
                    bosses[i].padraoAtual = GetRandomValue(0, MAX_PADROES_ATAQUE - 1);
                }
                
                // Executa o ataque baseado no padrão atual
                executarAtaqueBoss(&bosses[i]);
            }
            
            // Verifica se o boss foi derrotado
            if (bosses[i].vida <= 0) {
                bosses[i].ativo = false;
                pontuacao += 1000 * (i + 1); // Recompensa por derrotar o boss
                
                // Efeito visual de explosão (placeholder)
                for (int j = 0; j < 20; j++) {
                    // Aqui seria adicionado código para criar partículas de explosão
                }
            }
        }
    }
}

// Executa um ataque baseado no padrão atual do boss - Versão Bullet Hell
void executarAtaqueBoss(Boss* boss) {
    float tempo = GetTime();
    float fatorFase = boss->fase * 0.5f; // Fator de dificuldade baseado na fase
    
    // Cores baseadas na fase do boss para identidade visual
    Color corPrimaria, corSecundaria;
    
    switch (boss->fase) {
        case 1: // Primeiro boss - Vermelho/Laranja (Medo)
            corPrimaria = (Color){ 255, 50, 20, 255 };
            corSecundaria = (Color){ 255, 150, 0, 255 };
            break;
        case 2: // Segundo boss - Roxo/Azul (Ansiedade)
            corPrimaria = (Color){ 180, 20, 255, 255 };
            corSecundaria = (Color){ 50, 100, 255, 255 };
            break;
        case 3: // Terceiro boss - Vermelho/Preto (Desespero)
            corPrimaria = (Color){ 255, 0, 0, 255 };
            corSecundaria = (Color){ 20, 20, 20, 255 };
            break;
        default:
            corPrimaria = RED;
            corSecundaria = BLACK;
    }
    
    switch (boss->padraoAtual) {
        case PADRAO_CIRCULAR: {
            // Ataque em círculo INTENSO - várias camadas de projéteis
            int numCamadas = 3 + boss->fase;
            int projPorCamada = 12 + boss->fase * 3;
            
            for (int camada = 0; camada < numCamadas; camada++) {
                float raioBase = 6.0f + camada * 2.0f;
                float velocidadeBase = 3.0f + camada * 0.7f + fatorFase;
                float offset = camada * (PI / numCamadas) + boss->tempo * (1.0f + camada * 0.2f);
                
                for (int i = 0; i < projPorCamada; i++) {
                    float angulo = offset + (float)i * (2.0f * PI / projPorCamada);
                    Vector2 direcao = { cosf(angulo), sinf(angulo) };
                    
                    // Alterna cores para efeito visual
                    Color cor = (i % 2 == 0) ? corPrimaria : corSecundaria;
                    
                    // Tamanho e velocidade variam com a camada
                    float raio = raioBase - (camada * 0.5f);
                    float velocidade = velocidadeBase - (camada * 0.3f);
                    float dano = 8.0f + boss->fase * 4.0f;
                    
                    adicionarProjetil(boss, direcao, velocidade, raio, cor, dano);
                }
            }
            break;
        }
            
        case PADRAO_ESPIRAL: {
            // Ataque em espiral INTENSO - múltiplas espirais entrelaçadas
            int numEspirais = 3 + boss->fase;
            int projPorEspiral = 6;
            float velocidadeRotacao = 3.0f + boss->fase * 0.8f;
            
            for (int espiral = 0; espiral < numEspirais; espiral++) {
                float offsetAngulo = espiral * (2.0f * PI / numEspirais);
                float fatorVelocidade = 1.0f + (float)espiral * 0.15f;
                
                for (int i = 0; i < projPorEspiral; i++) {
                    float angulo = boss->tempo * velocidadeRotacao + offsetAngulo + (float)i * (2.0f * PI / projPorEspiral);
                    Vector2 direcao = { cosf(angulo), sinf(angulo) };
                    
                    // Alterna cores para efeito visual
                    Color cor = (espiral % 2 == 0) ? corPrimaria : corSecundaria;
                    cor.a = 200 + (unsigned char)(55.0f * sinf(tempo * 4.0f + espiral));
                    
                    float velocidade = (3.0f + fatorFase) * fatorVelocidade;
                    float raio = 5.0f + (espiral % 3) * 2.0f;
                    float dano = 7.0f + boss->fase * 3.0f;
                    
                    adicionarProjetil(boss, direcao, velocidade, raio, cor, dano);
                }
            }
            break;
        }
            
        case PADRAO_PERSEGUICAO: {
            // Ataque de perseguição INTENSO - múltiplos projéteis que seguem o jogador
            int numProjeteis = 5 + boss->fase * 2;
            float anguloBase = atan2f(posicaoCoracao.y - boss->posicao.y, posicaoCoracao.x - boss->posicao.x);
            
            for (int i = 0; i < numProjeteis; i++) {
                // Calcula direção para o jogador com variação
                float variacaoAngulo = (i - numProjeteis/2) * (PI / (8.0f + boss->fase));
                float angulo = anguloBase + variacaoAngulo;
                
                Vector2 direcao = { cosf(angulo), sinf(angulo) };
                
                // Projéteis mais rápidos e maiores para fases mais avançadas
                float velocidade = 4.5f + fatorFase + (float)i * 0.2f;
                float raio = 6.0f + (i % 3) * 2.0f;
                
                // Cor baseada na posição do projétil
                Color cor = (i % 3 == 0) ? corPrimaria : 
                           (i % 3 == 1) ? corSecundaria : 
                           (Color){ 255, 255, 255, 200 };
                
                float dano = 10.0f + boss->fase * 5.0f;
                
                adicionarProjetil(boss, direcao, velocidade, raio, cor, dano);
            }
            
            // Adiciona projéteis extras em círculo para dificultar a fuga
            if (boss->fase >= 2) {
                int numExtras = 8 + boss->fase * 2;
                for (int i = 0; i < numExtras; i++) {
                    float angulo = (float)i * (2.0f * PI / numExtras);
                    Vector2 direcao = { cosf(angulo), sinf(angulo) };
                    
                    float velocidade = 3.0f + fatorFase * 0.5f;
                    float raio = 4.0f;
                    Color cor = (Color){ 255, 100, 100, 200 };
                    float dano = 5.0f + boss->fase * 2.0f;
                    
                    adicionarProjetil(boss, direcao, velocidade, raio, cor, dano);
                }
            }
            break;
        }
            
        case PADRAO_ALEATORIO: {
            // Ataque aleatório INTENSO - caos total de projéteis
            int numProjeteis = 20 + boss->fase * 10;
            
            for (int i = 0; i < numProjeteis; i++) {
                float angulo = GetRandomValue(0, 360) * DEG2RAD;
                Vector2 direcao = { cosf(angulo), sinf(angulo) };
                
                // Velocidade e tamanho aleatórios para criar caos
                float velocidade = 2.0f + GetRandomValue(0, 40) / 10.0f + fatorFase;
                float raio = 3.0f + GetRandomValue(0, 8);
                
                // Cores aleatórias mas dentro da paleta do boss
                Color cor;
                if (GetRandomValue(0, 10) < 7) {
                    // Usa cores primárias/secundárias do boss
                    cor = (GetRandomValue(0, 1) == 0) ? corPrimaria : corSecundaria;
                    // Adiciona variação
                    cor.r = (unsigned char)Clamp(cor.r + GetRandomValue(-30, 30), 0, 255);
                    cor.g = (unsigned char)Clamp(cor.g + GetRandomValue(-30, 30), 0, 255);
                    cor.b = (unsigned char)Clamp(cor.b + GetRandomValue(-30, 30), 0, 255);
                } else {
                    // Cores totalmente aleatórias para alguns projéteis
                    cor = (Color){ 
                        (unsigned char)GetRandomValue(100, 255),
                        (unsigned char)GetRandomValue(100, 255),
                        (unsigned char)GetRandomValue(100, 255),
                        (unsigned char)GetRandomValue(200, 255)
                    };
                }
                
                float dano = 4.0f + boss->fase * 2.0f;
                
                adicionarProjetil(boss, direcao, velocidade, raio, cor, dano);
            }
            break;
        }
            
        case PADRAO_ONDAS: {
            // Ataque em ondas INTENSO - múltiplas ondas de projéteis
            int numLinhas = 8 + boss->fase * 2;
            int projPorLinha = 3 + boss->fase;
            
            // Cria várias linhas de projéteis
            for (int linha = 0; linha < numLinhas; linha++) {
                float y = -1.0f + linha * (2.0f / (numLinhas - 1));
                
                for (int i = 0; i < projPorLinha; i++) {
                    // Espaçamento entre projéteis na mesma linha
                    float offset = (float)i * (1.0f / projPorLinha) - 0.5f;
                    
                    // Direções para a direita e esquerda
                    Vector2 direcaoDireita = { 1.0f + offset * 0.2f, y + offset * 0.1f };
                    Vector2 direcaoEsquerda = { -1.0f - offset * 0.2f, y + offset * 0.1f };
                    
                    // Normaliza as direções
                    float comprimentoDir = sqrtf(direcaoDireita.x * direcaoDireita.x + direcaoDireita.y * direcaoDireita.y);
                    float comprimentoEsq = sqrtf(direcaoEsquerda.x * direcaoEsquerda.x + direcaoEsquerda.y * direcaoEsquerda.y);
                    
                    direcaoDireita.x /= comprimentoDir;
                    direcaoDireita.y /= comprimentoDir;
                    direcaoEsquerda.x /= comprimentoEsq;
                    direcaoEsquerda.y /= comprimentoEsq;
                    
                    // Velocidade aumenta com a fase
                    float velocidade = 3.5f + fatorFase + (float)i * 0.3f;
                    float raio = 5.0f + (linha % 3) * 1.5f;
                    
                    // Alterna cores para efeito visual
                    Color cor = (linha % 2 == 0) ? corPrimaria : corSecundaria;
                    float dano = 8.0f + boss->fase * 4.0f;
                    
                    // Adiciona projéteis em ambas direções
                    adicionarProjetil(boss, direcaoDireita, velocidade, raio, cor, dano);
                    adicionarProjetil(boss, direcaoEsquerda, velocidade, raio, cor, dano);
                }
            }
            
            // Adiciona projéteis extras em diagonal para fase 3
            if (boss->fase == 3) {
                for (int i = 0; i < 8; i++) {
                    float angulo = PI/4 + i * (PI/4);
                    Vector2 direcao = { cosf(angulo), sinf(angulo) };
                    
                    float velocidade = 5.0f;
                    float raio = 7.0f;
                    Color cor = (Color){ 255, 0, 0, 255 };
                    float dano = 15.0f;
                    
                    adicionarProjetil(boss, direcao, velocidade, raio, cor, dano);
                }
            }
            break;
        }
    }
}

// Adiciona um projétil ao boss
void adicionarProjetil(Boss* boss, Vector2 direcao, float velocidade, float raio, Color cor, float dano) {
    for (int i = 0; i < MAX_PROJETEIS; i++) {
        if (!boss->projeteis[i].ativo) {
            boss->projeteis[i].posicao = boss->posicao;
            boss->projeteis[i].velocidade.x = direcao.x * velocidade;
            boss->projeteis[i].velocidade.y = direcao.y * velocidade;
            boss->projeteis[i].raio = raio;
            boss->projeteis[i].cor = cor;
            boss->projeteis[i].ativo = true;
            boss->projeteis[i].dano = dano;
            break;
        }
    }
}

// Atualiza os projéteis de todos os bosses
void atualizarProjeteis(void) {
    for (int i = 0; i < MAX_BOSSES; i++) {
        if (bosses[i].ativo) {
            for (int j = 0; j < MAX_PROJETEIS; j++) {
                if (bosses[i].projeteis[j].ativo) {
                    // Atualiza a posição do projétil
                    bosses[i].projeteis[j].posicao.x += bosses[i].projeteis[j].velocidade.x;
                    bosses[i].projeteis[j].posicao.y += bosses[i].projeteis[j].velocidade.y;
                    
                    // Verifica se o projétil saiu da área de jogo
                    if (bosses[i].projeteis[j].posicao.x < AREA_JOGO_X ||
                        bosses[i].projeteis[j].posicao.x > AREA_JOGO_X + AREA_JOGO_LARGURA ||
                        bosses[i].projeteis[j].posicao.y < AREA_JOGO_Y ||
                        bosses[i].projeteis[j].posicao.y > AREA_JOGO_Y + AREA_JOGO_ALTURA) {
                        bosses[i].projeteis[j].ativo = false;
                    }
                    
                    // Verifica colisão com o coração
                    if (verificarColisaoProjetil(posicaoCoracao, bosses[i].projeteis[j])) {
                        // Guarda o valor anterior da vida
                        float vidaAnterior = vidaCoracao;
                        
                        float danoCausado = bosses[i].projeteis[j].dano;
                        vidaCoracao -= danoCausado;
                        bosses[i].projeteis[j].ativo = false;
                        
                        // Atualiza variáveis para efeito visual de dano
                        extern float efeitoDanoTempo;
                        extern float ultimoDano;
                        ultimoDano = vidaAnterior - vidaCoracao;
                        efeitoDanoTempo = 1.0f; // Duração do efeito em segundos
                        
                        // Cria número de dano flutuante
                        extern void adicionarNumeroDano(float valor, Vector2 posicao, bool ehDano);
                        adicionarNumeroDano(danoCausado, posicaoCoracao, true);
                    }
                }
            }
        }
    }
}

// Verifica se um projétil colidiu com o coração
// Verifica se um projétil colidiu com o coração
int verificarColisaoProjetil(Vector2 posicaoCoracao, Projetil projetil) {
    if (!projetil.ativo) return false;
    
    // Calcula a distância entre o centro do coração e o centro do projétil
    float distancia = sqrtf(
        powf(posicaoCoracao.x + TAMANHO_CORACAO/2 - projetil.posicao.x, 2) +
        powf(posicaoCoracao.y + TAMANHO_CORACAO/2 - projetil.posicao.y, 2)
    );
    
    // Verifica se a distância é menor que a soma dos raios
    return distancia < (TAMANHO_CORACAO/2 + projetil.raio);
}

// Desenha os bosses com efeitos visuais assustadores e impressionantes
void desenharBosses(void) {
    float tempo = GetTime();
    
    for (int i = 0; i < MAX_BOSSES; i++) {
        if (bosses[i].ativo) {
            // Cores e efeitos baseados na fase do boss
            Color corAura;
            float velocidadeDistorcao, intensidadePulsacao;
            // Removidas variáveis não utilizadas
            
            // Definimos corSecundaria como uma variável estática para ser usada em outras funções

            
            switch (i) {
                case 0: // Primeiro boss - MEDO (Vermelho/Preto)
                    corAura = (Color){ 
                        230 + (int)(25 * sinf(tempo * 5.0f)), 
                        0, 
                        0, 
                        150 + (int)(50 * sinf(tempo * 4.0f))
                    };
                    velocidadeDistorcao = 4.0f;
                    intensidadePulsacao = 0.3f;
                    break;
                    
                case 1: // Segundo boss - ANSIEDADE (Roxo/Azul)
                    corAura = (Color){ 
                        130 + (int)(50 * sinf(tempo * 3.0f)), 
                        0, 
                        200 + (int)(55 * sinf(tempo * 2.0f)), 
                        170 + (int)(40 * sinf(tempo * 3.5f))
                    };
                    velocidadeDistorcao = 5.0f;
                    intensidadePulsacao = 0.25f;
                    break;
                    
                case 2: // Terceiro boss - DESESPERO (Preto/Vermelho)
                    corAura = (Color){ 
                        150 + (int)(105 * fabsf(sinf(tempo * 6.0f))), 
                        0, 
                        0, 
                        200 + (int)(55 * fabsf(sinf(tempo * 8.0f)))
                    };
                    velocidadeDistorcao = 6.0f;
                    intensidadePulsacao = 0.4f;
                    break;
                    
                default:
                    corAura = RED;
                    velocidadeDistorcao = 3.0f;
                    intensidadePulsacao = 0.2f;
            }
            
            
            // Partículas ao redor do boss removidas para limpar o visual
            // Mantendo apenas um pequeno indicador sutil
            if ((int)(tempo * 4) % 8 == 0) {
                float tamanho = 2.0f;
                Color corParticula = corAura;
                corParticula.a = 60; // Muito transparente
                
                Vector2 posParticula = {
                    bosses[i].posicao.x + GetRandomValue(-15, 15),
                    bosses[i].posicao.y + GetRandomValue(-15, 15)
                };
                
                DrawCircleV(posParticula, tamanho, corParticula);
            }
            
            // Aura reduzida e mais sutil
            float raioAuraBase = 10.0f;
            float raioAura = raioAuraBase + 3.0f * sinf(tempo * 2.0f);
            
            // Apenas uma camada de aura muito sutil
            Color corCamada = corAura;
            corCamada.a = 40; // Transparência alta para ser mais sutil
            DrawCircleV(bosses[i].posicao, raioAura, corCamada);
            
            // Efeito de distorção no boss
            float escalaBase = 1.0f + 0.1f * intensidadePulsacao;
            float escalaX = escalaBase + 0.2f * sinf(tempo * velocidadeDistorcao + i);
            float escalaY = escalaBase + 0.2f * sinf(tempo * (velocidadeDistorcao-1.0f) + i + 1.5f);
            
            // Desenha o boss com efeitos de distorção
            DrawTexturePro(
                bosses[i].textura,
                bosses[i].frameAtual,
                (Rectangle){ 
                    bosses[i].posicao.x - (bosses[i].frameAtual.width * escalaX)/2,
                    bosses[i].posicao.y - (bosses[i].frameAtual.height * escalaY)/2,
                    bosses[i].frameAtual.width * escalaX,
                    bosses[i].frameAtual.height * escalaY
                },
                (Vector2){ bosses[i].frameAtual.width * escalaX/2, bosses[i].frameAtual.height * escalaY/2 },
                sinf(bosses[i].tempo) * 10.0f, // Rotação mais intensa
                WHITE
            );
            
            // Efeito de texto flutuante assustador (apenas em momentos específicos)
            if ((int)(tempo * 3) % 5 == i) {
                const char* textoAssustador;
                Color corTexto;
                int tamanhoTexto;
                
                switch (i) {
                    case 0:
                        textoAssustador = "MEDO";
                        corTexto = (Color){ 255, 0, 0, 200 + (int)(55 * sinf(tempo * 5.0f)) };
                        tamanhoTexto = 30;
                        break;
                    case 1:
                        textoAssustador = "ANSIEDADE";
                        corTexto = (Color){ 200, 0, 255, 200 + (int)(55 * sinf(tempo * 4.0f)) };
                        tamanhoTexto = 28;
                        break;
                    case 2:
                        textoAssustador = "DESESPERO";
                        corTexto = (Color){ 255, 50, 50, 200 + (int)(55 * sinf(tempo * 6.0f)) };
                        tamanhoTexto = 35;
                        break;
                }
                
                // Efeito de texto distorcido com sombra
                for (int d = 0; d < 3; d++) {
                    int offsetX = GetRandomValue(-2, 2);
                    int offsetY = GetRandomValue(-2, 2);
                    Color corSombra = BLACK;
                    corSombra.a = 100 + d * 50;
                    
                    DrawText(textoAssustador, 
                             bosses[i].posicao.x - MeasureText(textoAssustador, tamanhoTexto)/2 + offsetX + d, 
                             bosses[i].posicao.y - 60 + offsetY + d, 
                             tamanhoTexto, 
                             corSombra);
                }
                
                // Texto principal com efeito de tremor
                DrawText(textoAssustador, 
                         bosses[i].posicao.x - MeasureText(textoAssustador, tamanhoTexto)/2 + GetRandomValue(-2, 2), 
                         bosses[i].posicao.y - 60 + GetRandomValue(-2, 2), 
                         tamanhoTexto, 
                         corTexto);
            }
            
            // Barra de vida do boss com efeito de pulsação e gradiente
            float pulsacao = 1.0f + 0.15f * sinf(tempo * 4.0f);
            float porcentagemVida = bosses[i].vida / bosses[i].vidaMaxima;
            int larguraBarra = 300;
            
            // Fundo da barra (escuro)
            DrawRectangle(
                AREA_JOGO_X + 10,
                AREA_JOGO_Y + 10,
                larguraBarra,
                20,
                (Color){ 20, 0, 0, 200 }
            );
            
            // Gradiente na barra de vida
            for (int px = 0; px < (int)(larguraBarra * porcentagemVida); px++) {
                float fator = (float)px / larguraBarra; // 0.0 a 1.0
                Color corGradiente;
                
                // Cores diferentes para cada boss
                switch (i) {
                    case 0: // Vermelho para laranja
                        corGradiente = (Color){
                            255,
                            (unsigned char)(100 * fator),
                            0,
                            255
                        };
                        break;
                    case 1: // Roxo para azul
                        corGradiente = (Color){
                            (unsigned char)(200 - 150 * fator),
                            0,
                            (unsigned char)(200 + 55 * fator),
                            255
                        };
                        break;
                    case 2: // Vermelho para preto
                        corGradiente = (Color){
                            (unsigned char)(255 - 200 * fator),
                            0,
                            0,
                            255
                        };
                        break;
                    default:
                        corGradiente = RED;
                }
                
                // Efeito de pulsação na cor
                corGradiente.r = (unsigned char)Clamp(corGradiente.r + (int)(30 * sinf(tempo * 5.0f + fator * 10)), 0, 255);
                corGradiente.g = (unsigned char)Clamp(corGradiente.g + (int)(20 * sinf(tempo * 4.0f + fator * 8)), 0, 255);
                corGradiente.b = (unsigned char)Clamp(corGradiente.b + (int)(20 * sinf(tempo * 6.0f + fator * 12)), 0, 255);
                
                DrawRectangle(AREA_JOGO_X + 10 + px, AREA_JOGO_Y + 10, 1, 20, corGradiente);
            }
            
            // Borda da barra de vida com efeito pulsante
            DrawRectangleLines(
                AREA_JOGO_X + 10 - (int)(2 * pulsacao),
                AREA_JOGO_Y + 10 - (int)(2 * pulsacao),
                larguraBarra + (int)(4 * pulsacao),
                20 + (int)(4 * pulsacao),
                WHITE
            );
            
            // Nome do boss com efeito de tremor e sombra
            int offsetX = GetRandomValue(-2, 2);
            int offsetY = GetRandomValue(-2, 2);
            
            // Sombra do texto
            DrawText(
                bosses[i].nome,
                AREA_JOGO_X + 15 + offsetX + 2,
                AREA_JOGO_Y + 12 + offsetY + 2,
                20,
                BLACK
            );
            
            // Texto principal com cor baseada no boss
            Color corNome;
            switch (i) {
                case 0: corNome = (Color){ 255, 100, 50, 255 }; break;
                case 1: corNome = (Color){ 200, 100, 255, 255 }; break;
                case 2: corNome = (Color){ 255, 50, 50, 255 }; break;
                default: corNome = WHITE;
            }
            
            DrawText(
                bosses[i].nome,
                AREA_JOGO_X + 15 + offsetX,
                AREA_JOGO_Y + 12 + offsetY,
                20,
                corNome
            );
        }
    }
}

// Desenha os projéteis de todos os bosses
void desenharProjeteis(void) {
    for (int i = 0; i < MAX_BOSSES; i++) {
        if (bosses[i].ativo) {
            for (int j = 0; j < MAX_PROJETEIS; j++) {
                if (bosses[i].projeteis[j].ativo) {
                    // Desenha o projétil
                    DrawCircleV(
                        bosses[i].projeteis[j].posicao,
                        bosses[i].projeteis[j].raio,
                        bosses[i].projeteis[j].cor
                    );
                    
                    // Adiciona um efeito de brilho
                    DrawCircleV(
                        bosses[i].projeteis[j].posicao,
                        bosses[i].projeteis[j].raio * 0.6f,
                        (Color){ 255, 255, 255, 200 }
                    );
                }
            }
        }
    }
}

// Ativa o boss da fase atual
void ativarBossDaFase(int fase) {
    if (fase > 0 && fase <= MAX_BOSSES) {
        bosses[fase - 1].ativo = true;
        bosses[fase - 1].tempo = 0.0f;
        bosses[fase - 1].tempoAtaque = 0.0f;
        
        // Restaura a vida do boss se ele já foi derrotado antes
        if (bosses[fase - 1].vida <= 0) {
            bosses[fase - 1].vida = bosses[fase - 1].vidaMaxima;
        }
        
        printf("Boss da fase %d ativado: %s\n", fase, bosses[fase - 1].nome);
    }
}

// Define a dificuldade do jogo
void definirDificuldade(NivelDificuldade nivel) {
    dificuldadeAtual = nivel;
    
    // Ajusta parâmetros baseados na dificuldade
    switch (nivel) {
        case FACIL:
            for (int i = 0; i < MAX_BOSSES; i++) {
                bosses[i].vida = bosses[i].vidaMaxima * 0.7f;
                bosses[i].tempoEntreAtaques = 3.0f - (i * 0.3f);
            }
            break;
            
        case NORMAL:
            // Valores padrão
            for (int i = 0; i < MAX_BOSSES; i++) {
                bosses[i].vida = bosses[i].vidaMaxima;
                bosses[i].tempoEntreAtaques = 2.0f - (i * 0.3f);
            }
            break;
            
        case DIFICIL:
            for (int i = 0; i < MAX_BOSSES; i++) {
                bosses[i].vida = bosses[i].vidaMaxima * 1.3f;
                bosses[i].tempoEntreAtaques = 1.5f - (i * 0.2f);
            }
            break;
            
        case CUPHEAD:
            // Modo extremamente difícil inspirado em Cuphead
            for (int i = 0; i < MAX_BOSSES; i++) {
                bosses[i].vida = bosses[i].vidaMaxima * 1.5f;
                bosses[i].tempoEntreAtaques = 1.0f - (i * 0.15f);
                if (bosses[i].tempoEntreAtaques < 0.3f) {
                    bosses[i].tempoEntreAtaques = 0.3f; // Limite mínimo para não ser impossível
                }
            }
            break;
    }
    
    printf("Dificuldade definida para: %d\n", nivel);
}
