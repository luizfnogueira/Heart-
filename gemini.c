#include "gemini.h"
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include "raylib.h"
#include <stdio.h>
#include <json-c/json.h>


#define GEMINI_API_KEY "AIzaSyCFJL-R1U7Gy9UOy2VcAEt_diTANXM4oIw"

// Variável global para a mensagem atual
MensagemConforto mensagemAtual;

// Função auxiliar para receber resposta do curl
static size_t write_callback(void *contents, size_t size, size_t nmemb, void *userp) {
    size_t realsize = size * nmemb;
    strncat((char *)userp, (char *)contents, realsize);
    return realsize;
}

// Inicialização global da libcurl
void inicializarGemini(void) {
    CURLcode res = curl_global_init(CURL_GLOBAL_DEFAULT);
    if (res != CURLE_OK) {
        printf("[GEMINI] Falha ao inicializar libcurl: %s\n", curl_easy_strerror(res));
    } else {
        printf("[GEMINI] libcurl inicializada com sucesso!\n");
    }
}

// Função para pedir resposta da IA Gemini
void pedirGemini(const char* prompt, char* resposta, size_t resposta_tam) {
    printf("[GEMINI] Fazendo requisição para Gemini: %s\n", prompt);
    CURL *curl = curl_easy_init();
    if (!curl) { printf("[GEMINI] Erro ao inicializar CURL\n"); return; }

    char url[512];
    snprintf(url, sizeof(url),
        "https://generativelanguage.googleapis.com/v1beta/models/gemini-2.0-flash:generateContent?key=%s",
        GEMINI_API_KEY);

    char postdata[1024];
    snprintf(postdata, sizeof(postdata),
        "{\"contents\":[{\"parts\":[{\"text\":\"%s\"}]}]}",
        prompt);

    struct curl_slist *headers = NULL;
    headers = curl_slist_append(headers, "Content-Type: application/json");

    resposta[0] = 0;
    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, postdata);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, resposta);

    CURLcode res = curl_easy_perform(curl);
    if (res != CURLE_OK) {
        snprintf(resposta, resposta_tam, "Erro: %s", curl_easy_strerror(res));
        printf("[GEMINI] Erro na requisição: %s\n", curl_easy_strerror(res));
    } else {
        printf("[GEMINI] Resposta recebida: %s\n", resposta);
    }

    curl_easy_cleanup(curl);
    curl_slist_free_all(headers);
}

// Inicializar mensagens de conforto
void inicializarMensagensConforto(void) {
    memset(&mensagemAtual, 0, sizeof(MensagemConforto));
    mensagemAtual.ativa = true;
    strcpy(mensagemAtual.mensagem, "Lembre-se: mesmo nas trevas, há uma luz que guia seu coração.");
}

// Atualizar mensagem de conforto
void atualizarMensagemConforto(void) {
    char resposta[4096] = "";
    pedirGemini("Me dê uma mensagem de conforto curta, de poucas frases, no estilo: 'Seu coração anseia.'", resposta, sizeof(resposta));

    struct json_object *parsed_json = json_tokener_parse(resposta);
    const char *mensagem = NULL;
    if (parsed_json) {
        struct json_object *candidates = NULL;
        if (json_object_object_get_ex(parsed_json, "candidates", &candidates) && json_object_is_type(candidates, json_type_array)) {
            struct json_object *primeiro = json_object_array_get_idx(candidates, 0);
            if (primeiro) {
                struct json_object *content = NULL;
                if (json_object_object_get_ex(primeiro, "content", &content)) {
                    struct json_object *parts = NULL;
                    if (json_object_object_get_ex(content, "parts", &parts) && json_object_is_type(parts, json_type_array)) {
                        struct json_object *part0 = json_object_array_get_idx(parts, 0);
                        if (part0) {
                            struct json_object *text = NULL;
                            if (json_object_object_get_ex(part0, "text", &text)) {
                                mensagem = json_object_get_string(text);
                            }
                        }
                    }
                }
            }
        }
        json_object_put(parsed_json);
    }
    if (mensagem && strlen(mensagem) > 0) {
        // Limitar a mensagem a poucas frases (até o primeiro ponto final, interrogação ou exclamação)
        size_t len = strcspn(mensagem, ".!?\n");
        if (mensagem[len] != '\0') len++; // inclui a pontuação
        if (len > 0 && len < sizeof(mensagemAtual.mensagem)) {
            strncpy(mensagemAtual.mensagem, mensagem, len);
            mensagemAtual.mensagem[len] = '\0';
        } else {
            strncpy(mensagemAtual.mensagem, mensagem, sizeof(mensagemAtual.mensagem)-1);
            mensagemAtual.mensagem[sizeof(mensagemAtual.mensagem)-1] = '\0';
        }
    } else {
        strncpy(mensagemAtual.mensagem, "Seu coração anseia.", sizeof(mensagemAtual.mensagem)-1);
        mensagemAtual.mensagem[sizeof(mensagemAtual.mensagem)-1] = '\0';
    }
    mensagemAtual.ativa = 1;
}

// Desenhar mensagem de conforto
void desenharMensagemConforto(void) {
    if (!mensagemAtual.ativa) return;
    int larguraTela = GetScreenWidth();
    int alturaTela = GetScreenHeight();
    int larguraTexto = MeasureText(mensagemAtual.mensagem, 20);
    // Exibir centralizado no rodapé, 20px acima da borda inferior
    DrawText(mensagemAtual.mensagem, (larguraTela - larguraTexto) / 2, alturaTela - 30, 20, LIGHTGRAY);
}

// Liberar recursos
void liberarRecursosMensagemConforto(void) {
    // Nenhum recurso dinâmico para liberar no momento
}
