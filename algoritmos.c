#include "algoritmos.h"
#include <math.h>
#include <stdlib.h>
#include <float.h>

// Função auxiliar para calcular a distância entre dois pontos
static float calcularDistancia(Vector2 p1, Vector2 p2) {
    float dx = p2.x - p1.x;
    float dy = p2.y - p1.y;
    return sqrtf(dx*dx + dy*dy);
}

// Implementação do algoritmo de ordenação QuickSort para ordenar obstáculos por proximidade
static int particionar(Obstaculo* obstaculos, int inicio, int fim, Vector2 posicaoJogador) {
    // Usando o último elemento como pivô
    float distanciaPivo = calcularDistancia(obstaculos[fim].posicao, posicaoJogador);
    int i = inicio - 1;
    
    for (int j = inicio; j <= fim - 1; j++) {
        float distanciaAtual = calcularDistancia(obstaculos[j].posicao, posicaoJogador);
        
        // Se o elemento atual estiver mais próximo que o pivô
        if (distanciaAtual < distanciaPivo) {
            i++;
            // Trocar obstaculos[i] e obstaculos[j]
            Obstaculo temp = obstaculos[i];
            obstaculos[i] = obstaculos[j];
            obstaculos[j] = temp;
        }
    }
    
    // Trocar obstaculos[i+1] e obstaculos[fim] (pivô)
    Obstaculo temp = obstaculos[i + 1];
    obstaculos[i + 1] = obstaculos[fim];
    obstaculos[fim] = temp;
    
    return (i + 1);
}

static void quickSort(Obstaculo* obstaculos, int inicio, int fim, Vector2 posicaoJogador) {
    if (inicio < fim) {
        int indicePivo = particionar(obstaculos, inicio, fim, posicaoJogador);
        
        // Ordenar os elementos antes e depois do pivô
        quickSort(obstaculos, inicio, indicePivo - 1, posicaoJogador);
        quickSort(obstaculos, indicePivo + 1, fim, posicaoJogador);
    }
}

// Função para ordenar obstáculos por proximidade com o jogador
void ordenarObstaculosPorProximidade(Obstaculo* obstaculos, int tamanho, Vector2 posicaoJogador) {
    if (tamanho <= 1) return;
    
    // Filtrar apenas obstáculos ativos
    Obstaculo* obstaculosAtivos = (Obstaculo*)malloc(tamanho * sizeof(Obstaculo));
    int contadorAtivos = 0;
    
    for (int i = 0; i < tamanho; i++) {
        if (obstaculos[i].ativo) {
            obstaculosAtivos[contadorAtivos++] = obstaculos[i];
        }
    }
    
    // Aplicar QuickSort nos obstáculos ativos
    if (contadorAtivos > 1) {
        quickSort(obstaculosAtivos, 0, contadorAtivos - 1, posicaoJogador);
    }
    
    // Copiar de volta para o array original, mantendo obstáculos inativos
    int indexAtivo = 0;
    for (int i = 0; i < tamanho; i++) {
        if (obstaculos[i].ativo && indexAtivo < contadorAtivos) {
            obstaculos[i] = obstaculosAtivos[indexAtivo++];
        }
    }
    
    free(obstaculosAtivos);
}

// Função para encontrar um caminho seguro entre obstáculos
Vector2 encontrarCaminhoSeguro(Vector2 posicaoJogador, Obstaculo* obstaculos, int tamanho) {
    // Define uma grade de pontos possíveis para o jogador se mover
    const int GRID_SIZE = 8;
    const float STEP = 25.0f;
    
    Vector2 melhorPosicao = posicaoJogador;
    float maiorDistanciaSegura = 0.0f;
    
    // Testar posições ao redor do jogador
    for (int i = 0; i < GRID_SIZE; i++) {
        float angulo = (float)i * (2.0f * PI / GRID_SIZE);
        
        Vector2 novaPosicao = {
            posicaoJogador.x + STEP * cosf(angulo),
            posicaoJogador.y + STEP * sinf(angulo)
        };
        
        // Verificar se a posição está dentro dos limites da área de jogo
        if (novaPosicao.x < AREA_JOGO_X + TAMANHO_CORACAO/2 || 
            novaPosicao.x > AREA_JOGO_X + AREA_JOGO_LARGURA - TAMANHO_CORACAO/2 ||
            novaPosicao.y < AREA_JOGO_Y + TAMANHO_CORACAO/2 || 
            novaPosicao.y > AREA_JOGO_Y + AREA_JOGO_ALTURA - TAMANHO_CORACAO/2) {
            continue;
        }
        
        // Calcular a distância mínima aos obstáculos
        float distanciaMinima = FLT_MAX;
        
        for (int j = 0; j < tamanho; j++) {
            if (obstaculos[j].ativo) {
                float distancia = calcularDistancia(novaPosicao, obstaculos[j].posicao);
                
                // Ajustar distância com base no tamanho e tipo do obstáculo
                float escalaVertical = 0.35f;
                float ajuste = (obstaculos[j].comprimento == 10) ? 
                              76 * escalaVertical / 2 : 40 * escalaVertical / 2;
                
                distancia -= ajuste;
                
                if (distancia < distanciaMinima) {
                    distanciaMinima = distancia;
                }
            }
        }
        
        // Se esta posição é mais segura que a melhor encontrada até agora
        if (distanciaMinima > maiorDistanciaSegura) {
            maiorDistanciaSegura = distanciaMinima;
            melhorPosicao = novaPosicao;
        }
    }
    
    // Se encontramos um lugar seguro
    if (maiorDistanciaSegura > TAMANHO_CORACAO * 1.5f) {
        return melhorPosicao;
    }
    
    // Caso não encontre um lugar suficientemente seguro, sugere manter a posição
    return posicaoJogador;
}
