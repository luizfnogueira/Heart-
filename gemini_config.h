#ifndef GEMINI_CONFIG_H
#define GEMINI_CONFIG_H

#include <stdbool.h>

// Informações do projeto acadêmico
#define EMAIL_CONTATO "tcq@cesar.school"
#define TURMA "Turma B"

// Nome do jogo e descrição
#define NOME_JOGO "Heart"
#define DESCRICAO_JOGO "Um jogo de ação onde você controla um coração que deve navegar por obstáculos e enfrentar bosses que representam emoções e memórias."

// Objetivo do jogo
#define OBJETIVO_JOGO "Sobreviver aos obstáculos, derrotar os bosses e alcançar a pontuação máxima."

// Estruturas de dados utilizadas
/*
 * O jogo utiliza as seguintes estruturas de dados:
 * 1. Arrays para armazenar obstáculos e projéteis
 * 2. Listas encadeadas para gerenciar efeitos visuais
 * 3. Árvores binárias para o sistema de decisão dos bosses
 * 4. Filas para gerenciar eventos e animações
 */

// Algoritmos de ordenação utilizados
/*
 * 1. Quick Sort - Utilizado para ordenar projéteis por proximidade ao jogador
 * 2. Merge Sort - Utilizado para ordenar obstáculos por tipo e posição
 * 3. Heap Sort - Utilizado para ordenar pontuações no ranking
 */

// API de IA utilizada
#define API_IA "Gemini API"
#define API_URL "https://generativelanguage.googleapis.com/v1beta/models/gemini-pro:generateContent"

// Funcionalidades relacionadas à IA
/*
 * 1. Comportamento adaptativo dos bosses baseado na performance do jogador
 * 2. Geração de padrões de ataque dinâmicos
 * 3. Ajuste automático de dificuldade
 * 4. Narrativa gerada proceduralmente
 */

// Interface gráfica
#define TEM_INTERFACE_GRAFICA 1  // 1 = Sim, 0 = Não

// Configurações de dificuldade inspiradas em Cuphead
typedef enum {
    DIFICULDADE_FACIL,
    DIFICULDADE_NORMAL,
    DIFICULDADE_DIFICIL,
    DIFICULDADE_CUPHEAD
} DificuldadeJogo;

// Estrutura para configuração da API Gemini
typedef struct {
    char apiKey[100];
    char modelo[50];
    float temperatura;
    int maxTokens;
    bool ativa;
} ConfiguracaoGemini;

// Função para inicializar a configuração da API Gemini
void inicializarConfigGemini(ConfiguracaoGemini* config);

// Função para validar a configuração da API Gemini
bool validarConfigGemini(ConfiguracaoGemini config);

// Função para exibir informações sobre o projeto acadêmico
void exibirInformacoesProjetoAcademico(void);

#endif // GEMINI_CONFIG_H
