#ifndef ALGORITMOS_H
#define ALGORITMOS_H

#include "jogo.h"

// Algoritmo de ordenação para obstáculos com base na proximidade com o jogador
void ordenarObstaculosPorProximidade(Obstaculo* obstaculos, int tamanho, Vector2 posicaoJogador);

// Algoritmo para encontrar o caminho mais seguro entre obstáculos
Vector2 encontrarCaminhoSeguro(Vector2 posicaoJogador, Obstaculo* obstaculos, int tamanho);

#endif // ALGORITMOS_H
