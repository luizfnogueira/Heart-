#include "jogo.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>

// Função auxiliar para duplicar strings (compatível com Windows e Unix)
static char* duplicar_string(const char* s) {
    size_t len = strlen(s) + 1;
    char* p = malloc(len);
    if (p) {
        memcpy(p, s, len);
    }
    return p;
}

// Chave da API Gemini
static const char* GEMINI_API_KEY = "AIzaSyCFJL-R1U7Gy9UOy2VcAEt_diTANXM4oIw";

// Usamos a estrutura ApiResponse definida em jogo.h

// Callback para receber os dados da resposta HTTP
static size_t escreverCallback(void* contents, size_t size, size_t nmemb, void* userp) {
    size_t realSize = size * nmemb;
    ApiResponse* resp = (ApiResponse*)userp;
    
    char* ptr = realloc(resp->data, resp->size + realSize + 1);
    if (!ptr) {
        printf("Erro: sem memória suficiente!\n");
        return 0;
    }
    
    resp->data = ptr;
    memcpy(&(resp->data[resp->size]), contents, realSize);
    resp->size += realSize;
    resp->data[resp->size] = 0;
    
    return realSize;
}

// Função simplificada para consultar a API Gemini (sem dependência de jansson)
char* consultarGeminiAPI(const char* prompt, const char* contexto) {
    CURL* curl;
    CURLcode res;
    ApiResponse resposta = {0};
    
    resposta.data = malloc(1);
    resposta.size = 0;
    
    curl = curl_easy_init();
    if (!curl) {
        free(resposta.data);
        return duplicar_string("Erro ao inicializar CURL");
    }
    
    // Formatar o JSON para a requisição
    char jsonBuffer[4096];
    snprintf(jsonBuffer, sizeof(jsonBuffer),
             "{"
             "\"contents\": ["
             "{"
             "\"parts\": ["
             "{"
             "\"text\": \"Contexto do jogo: %s\\n\\nPrompt: %s\\n\\nResponda apenas com a estratégia de ataque que o boss deve usar. Seja breve e direto.\""
             "}"
             "]"
             "}"
             "],"
             "\"generationConfig\": {"
             "\"temperature\": 0.7,"
             "\"maxOutputTokens\": 100"
             "}"
             "}", 
             contexto, prompt);
    
    // Configurar a URL com a chave da API
    char urlCompleta[256];
    snprintf(urlCompleta, sizeof(urlCompleta), 
             "%s?key=%s", 
             GEMINI_API_URL, GEMINI_API_KEY);
    
    // Configurar os headers
    struct curl_slist* headers = NULL;
    headers = curl_slist_append(headers, "Content-Type: application/json");
    
    // Configurar a requisição
    curl_easy_setopt(curl, CURLOPT_URL, urlCompleta);
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, jsonBuffer);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, escreverCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void*)&resposta);
    
    // Realizar a requisição
    res = curl_easy_perform(curl);
    
    // Verificar erros
    if (res != CURLE_OK) {
        fprintf(stderr, "Erro na requisição: %s\n", curl_easy_strerror(res));
        free(resposta.data);
        curl_slist_free_all(headers);
        curl_easy_cleanup(curl);
        return duplicar_string("Erro na requisição à API");
    }
    
    // Versão simplificada para extrair o texto da resposta sem usar jansson
    // Apenas para fins de demonstração do projeto acadêmico
    char* textoResposta = duplicar_string("circular"); // Padrão default para demonstração
    
    // Busca por padrões de ataque na resposta bruta
    if (strstr(resposta.data, "circular") != NULL) {
        textoResposta = duplicar_string("circular");
    } else if (strstr(resposta.data, "espiral") != NULL) {
        textoResposta = duplicar_string("espiral");
    } else if (strstr(resposta.data, "persegui") != NULL) {
        textoResposta = duplicar_string("perseguicao");
    } else if (strstr(resposta.data, "aleat") != NULL) {
        textoResposta = duplicar_string("aleatorio");
    } else if (strstr(resposta.data, "onda") != NULL) {
        textoResposta = duplicar_string("ondas");
    }
    
    // Limpar
    free(resposta.data);
    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);
    
    printf("API Gemini consultada. Email de contato: %s\n", EMAIL_CONTATO);
    printf("Resposta: %s\n", textoResposta);
    
    return textoResposta;
}

// Função para determinar o próximo ataque do boss com base na IA
void determinarProximoAtaqueBoss(Boss* boss) {
    // Declarando as variáveis externas que precisamos acessar
    extern float vidaCoracao;
    extern int faseAtual;
    extern float pontuacao;
    
    // Criar contexto com informações do jogo
    char contexto[512];
    snprintf(contexto, sizeof(contexto),
             "Você é um boss em um jogo de ação chamado Heart. "
             "O jogador controla um coração vermelho que deve desviar de obstáculos. "
             "Sua vida atual é %.1f/%.1f. A vida do jogador é %.1f. "
             "Fase atual: %d. Pontuação do jogador: %.1f",
             boss->vida, boss->vidaMaxima, vidaCoracao, faseAtual, pontuacao);
    
    // Criar prompt para a IA
    char prompt[512];
    snprintf(prompt, sizeof(prompt),
             "Como boss, qual padrão de ataque devo usar contra o jogador? "
             "Opções: circular, espiral, perseguição, aleatório, ondas. "
             "Considere a vida atual do jogador e a minha própria vida para escolher "
             "a estratégia mais eficaz. Seja implacável mas justo.");
    
    // Consultar a API
    char* resposta = consultarGeminiAPI(prompt, contexto);
    
    // Armazenar a resposta
    strncpy(boss->ultimaResposta, resposta, sizeof(boss->ultimaResposta) - 1);
    boss->ultimaResposta[sizeof(boss->ultimaResposta) - 1] = '\0';
    
    // Determinar o padrão de ataque com base na resposta
    if (strstr(resposta, "circular") != NULL) {
        boss->padraoAtual = PADRAO_CIRCULAR;
    } else if (strstr(resposta, "espiral") != NULL) {
        boss->padraoAtual = PADRAO_ESPIRAL;
    } else if (strstr(resposta, "perseguição") != NULL || strstr(resposta, "perseguicao") != NULL) {
        boss->padraoAtual = PADRAO_PERSEGUICAO;
    } else if (strstr(resposta, "aleatório") != NULL || strstr(resposta, "aleatorio") != NULL) {
        boss->padraoAtual = PADRAO_ALEATORIO;
    } else if (strstr(resposta, "ondas") != NULL) {
        boss->padraoAtual = PADRAO_ONDAS;
    } else {
        // Padrão default se a resposta não for reconhecida
        boss->padraoAtual = PADRAO_CIRCULAR;
    }
    
    free(resposta);
}

// Função para inicializar a biblioteca CURL
void inicializarIA(void) {
    curl_global_init(CURL_GLOBAL_ALL);
    printf("Sistema de IA Gemini inicializado. Email de contato: %s\n", EMAIL_CONTATO);
}

// Função para limpar recursos da biblioteca CURL
void finalizarIA(void) {
    curl_global_cleanup();
}
