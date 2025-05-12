#include "gemini_config.h"
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

// Implementação da função para inicializar a configuração da API Gemini
void inicializarConfigGemini(ConfiguracaoGemini* config) {
    if (config == NULL) return;
    
    // Inicializa com valores padrão
    strcpy(config->apiKey, "AIzaSyCFJL-R1U7Gy9UOy2VcAEt_diTANXM4oIw");
    strcpy(config->modelo, "gemini-pro");
    config->temperatura = 0.7f;
    config->maxTokens = 100;
    config->ativa = true;
    
    printf("Configuração da API Gemini inicializada.\n");
    printf("Email de contato: %s\n", EMAIL_CONTATO);
    printf("Turma: %s\n", TURMA);
    printf("Nome do jogo: %s\n", NOME_JOGO);
    
    // Informações sobre o projeto acadêmico
    printf("\n=== Informações do Projeto Acadêmico ===\n");
    printf("O jogo %s utiliza a API %s para controlar o comportamento dos bosses.\n", NOME_JOGO, API_IA);
    printf("Objetivo: %s\n", OBJETIVO_JOGO);
    printf("Interface gráfica: %s\n", TEM_INTERFACE_GRAFICA ? "Sim" : "Não");
    
    printf("\nEstrutura de dados principais:\n");
    printf("- Arrays para armazenar obstáculos e projéteis\n");
    printf("- Listas encadeadas para gerenciar efeitos visuais\n");
    printf("- Árvores binárias para o sistema de decisão dos bosses\n");
    printf("- Filas para gerenciar eventos e animações\n");
    
    printf("\nAlgoritmos de ordenação utilizados:\n");
    printf("- Quick Sort: Ordenação de projéteis por proximidade\n");
    printf("- Merge Sort: Ordenação de obstáculos por tipo e posição\n");
    printf("- Heap Sort: Ordenação de pontuações no ranking\n");
    
    printf("\nFuncionalidades relacionadas à IA:\n");
    printf("- Comportamento adaptativo dos bosses baseado na performance do jogador\n");
    printf("- Geração de padrões de ataque dinâmicos\n");
    printf("- Ajuste automático de dificuldade\n");
    printf("- Narrativa gerada proceduralmente\n");
}

// Implementação da função para validar a configuração da API Gemini
bool validarConfigGemini(ConfiguracaoGemini config) {
    // Verifica se a chave API foi definida
    if (strcmp(config.apiKey, "SUA_CHAVE_API_AQUI") == 0) {
        printf("AVISO: Chave API não configurada. A integração com Gemini não funcionará.\n");
        return false;
    }
    
    // Verifica se o modelo é válido
    if (strcmp(config.modelo, "gemini-pro") != 0 && 
        strcmp(config.modelo, "gemini-pro-vision") != 0) {
        printf("ERRO: Modelo Gemini inválido. Use 'gemini-pro' ou 'gemini-pro-vision'.\n");
        return false;
    }
    
    // Verifica se a temperatura está dentro do intervalo válido
    if (config.temperatura < 0.0f || config.temperatura > 1.0f) {
        printf("ERRO: Temperatura deve estar entre 0.0 e 1.0.\n");
        return false;
    }
    
    // Verifica se maxTokens é válido
    if (config.maxTokens <= 0 || config.maxTokens > 2048) {
        printf("ERRO: maxTokens deve estar entre 1 e 2048.\n");
        return false;
    }
    
    printf("Configuração da API Gemini validada com sucesso!\n");
    return true;
}

// Função para exibir informações sobre o projeto acadêmico
void exibirInformacoesProjetoAcademico(void) {
    printf("\n===== PROJETO ACADÊMICO =====\n");
    printf("Nome do jogo: %s\n", NOME_JOGO);
    printf("Email de contato: %s\n", EMAIL_CONTATO);
    printf("Turma: %s\n", TURMA);
    
    printf("\nIntrodução:\n%s\n", DESCRICAO_JOGO);
    printf("\nObjetivo:\n%s\n", OBJETIVO_JOGO);
    
    printf("\nEstrutura de dados utilizada:\n");
    printf("- Arrays para armazenar obstáculos e projéteis\n");
    printf("- Listas encadeadas para gerenciar efeitos visuais\n");
    printf("- Árvores binárias para o sistema de decisão dos bosses\n");
    printf("- Filas para gerenciar eventos e animações\n");
    
    printf("\nJustificativa das estruturas de dados:\n");
    printf("- Arrays: Eficientes para acesso rápido a elementos e iteração sequencial\n");
    printf("- Listas encadeadas: Flexíveis para adição/remoção dinâmica de efeitos visuais\n");
    printf("- Árvores binárias: Ideais para sistemas de decisão hierárquicos dos bosses\n");
    printf("- Filas: Perfeitas para processamento ordenado de eventos e animações\n");
    
    printf("\nAlgoritmos de ordenação utilizados:\n");
    printf("- Quick Sort: Eficiente para ordenar grandes conjuntos de projéteis\n");
    printf("- Merge Sort: Estável e previsível para ordenação de obstáculos\n");
    printf("- Heap Sort: Ótimo para manter um ranking de pontuações atualizado\n");
    
    printf("\nJustificativa dos algoritmos de ordenação:\n");
    printf("- Quick Sort: O(n log n) em média, eficiente para ordenações frequentes\n");
    printf("- Merge Sort: O(n log n) garantido, estabilidade importante para preservar ordem original\n");
    printf("- Heap Sort: O(n log n) garantido, eficiente para manter estruturas parcialmente ordenadas\n");
    
    printf("\nAPI de IA utilizada: %s\n", API_IA);
    printf("URL da API: %s\n", API_URL);
    
    printf("\nFuncionalidades do jogo:\n");
    printf("- Movimento do coração controlado pelo jogador\n");
    printf("- Geração procedural de obstáculos\n");
    printf("- Sistema de fases progressivas\n");
    printf("- Bosses controlados por IA usando Gemini API\n");
    printf("- Sistema de pontuação e ranking\n");
    printf("- Efeitos visuais dinâmicos\n");
    printf("- Dificuldade ajustável (incluindo modo Cuphead)\n");
    
    printf("\nInterface gráfica: %s\n", TEM_INTERFACE_GRAFICA ? "Sim" : "Não");
    
    printf("\nReferências bibliográficas:\n");
    printf("- Raylib Documentation: https://www.raylib.com/\n");
    printf("- Google Gemini API: https://ai.google.dev/docs/gemini_api\n");
    
    printf("\nParticipação na Mostra TechDesign: Sim\n");
    printf("\n=============================\n");
}
