#include "charada.h"
#include "jogo.h"
#include "estruturas.h" // Incluindo as estruturas de dados
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>
#include <stdarg.h>
#include "raylib.h"
#include <json-c/json.h> // Biblioteca para manipular JSON

// Ativar suporte a curl
#define HAS_CURL 1 // Defina como 1 quando tiver curl instalado

// Define para ativar logs detalhados
#define DEBUG_LOGS 1

#if HAS_CURL
#include <curl/curl.h>
#endif

// Implementação da função registrarLog
void registrarLog(const char* formato, ...);

void DrawTextBoxedCentered(const char *text, Rectangle rec, int fontSize, int spacing, Color color);

// Declaração da variável global nivelDificuldade
extern NivelDificuldade nivelDificuldade;

// Referências externas às variáveis e funções do jogo.c
extern int faseAtual;
extern void mudarParaFase2(void);
extern void mudarParaFase3(void);

// Constantes para o sistema de log
#define LOG_INFO  0
#define LOG_WARN  1
#define LOG_ERROR 2
#define MAX_LOGS  5

// Estrutura para armazenar logs de charadas
typedef struct {
    char mensagem[1024];
    bool exibir;
    float tempo;
    int posY;
    int tipo; // 0=info, 1=warn, 2=error
} LogCharada;

// Logs múltiplos para histórico
typedef struct {
    LogCharada logs[MAX_LOGS];
    int currentLog;
    bool ativo;
} SistemaLog;

// Variáveis globais
Charada charadaAtual;
bool modoCharada = false;
ListaCharadas listaCharadas; // Lista encadeada para armazenar charadas
static Texture2D fundoCharada;
static char mensagemStatus[256] = "";
static float tempoFeedback = 0.0f;
static Rectangle caixaInput = {0};
static SistemaLog sistemaLog = {0};

// Implementação da função registrarLog
void registrarLog(const char* formato, ...) {
    va_list args;
    va_start(args, formato);
    char buffer[1024];
    vsnprintf(buffer, sizeof(buffer), formato, args);
#if DEBUG_LOGS
    int tipoLog = 0; // LOG_INFO padrão
    if (strstr(buffer, "ERRO")) tipoLog = 1; // LOG_ERROR
    else if (strstr(buffer, "AVISO")) tipoLog = 2; // LOG_WARN
    printf("[%s] %s\n", (tipoLog == 1) ? "ERRO" : (tipoLog == 2) ? "AVISO" : "INFO", buffer);
    int slotLivre = -1;
    for (int i = 0; i < MAX_LOGS; i++) {
        if (!sistemaLog.logs[i].exibir) { slotLivre = i; break; }
    }
    if (slotLivre == -1) {
        slotLivre = sistemaLog.currentLog;
        sistemaLog.currentLog = (sistemaLog.currentLog + 1) % MAX_LOGS;
    }
    strncpy(sistemaLog.logs[slotLivre].mensagem, buffer, sizeof(sistemaLog.logs[slotLivre].mensagem) - 1);
    sistemaLog.logs[slotLivre].mensagem[sizeof(sistemaLog.logs[slotLivre].mensagem) - 1] = '\0';
    sistemaLog.logs[slotLivre].exibir = true;
    sistemaLog.logs[slotLivre].tempo = 5.0f;
    sistemaLog.logs[slotLivre].posY = 150;
    sistemaLog.logs[slotLivre].tipo = tipoLog;
    sistemaLog.ativo = true;
    printf("[CHARADA LOG] %s\n", buffer);
#endif
    va_end(args);
}

#if HAS_CURL
static const char* GEMINI_API_KEY = "AIzaSyCFJL-R1U7Gy9UOy2VcAEt_diTANXM4oIw";
#endif

#if HAS_CURL
// Estrutura para armazenar a resposta da API
typedef struct {
    char *data;
    size_t size;
} ResponseData;

// Callback para processar a resposta da API
static size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp) {
    size_t realSize = size * nmemb;
    ResponseData *response = (ResponseData *)userp;
    
    char *ptr = realloc(response->data, response->size + realSize + 1);
    if(!ptr) {
        printf("Erro: Falha ao alocar memória\n");
        return 0;
    }
    
    response->data = ptr;
    memcpy(&(response->data[response->size]), contents, realSize);
    response->size += realSize;
    response->data[response->size] = 0;
    
    return realSize;
}

// Função para extrair texto de uma resposta JSON da API Gemini
bool extrairRespostaGemini(const char* jsonResponse, char* pergunta, char* resposta1, char* resposta2, char* resposta3) {
    struct json_object *parsed_json, *candidates, *content, *parts, *textObj, *textField;
    parsed_json = json_tokener_parse(jsonResponse);

    if (!parsed_json) {
        registrarLog("ERRO: Falha ao parsear a resposta JSON da API");
        return false;
    }

    if (!json_object_object_get_ex(parsed_json, "candidates", &candidates)) {
        registrarLog("ERRO: Campo 'candidates' ausente na resposta JSON");
        json_object_put(parsed_json);
        return false;
    }

    int num_candidates = json_object_array_length(candidates);
    if (num_candidates == 0) {
        registrarLog("ERRO: Nenhum candidato encontrado na resposta JSON");
        json_object_put(parsed_json);
        return false;
    }

    // Pega o primeiro candidato
    content = json_object_array_get_idx(candidates, 0);
    if (!json_object_object_get_ex(content, "content", &content)) {
        registrarLog("ERRO: Campo 'content' ausente em 'candidates[0]'");
        json_object_put(parsed_json);
        return false;
    }

    if (!json_object_object_get_ex(content, "parts", &parts)) {
        registrarLog("ERRO: Campo 'parts' ausente em 'content'");
        json_object_put(parsed_json);
        return false;
    }

    int num_parts = json_object_array_length(parts);
    if (num_parts == 0) {
        registrarLog("ERRO: Nenhum 'parts' encontrado em 'content'");
        json_object_put(parsed_json);
        return false;
    }

    // Pega o primeiro part e extrai o campo "text"
    textObj = json_object_array_get_idx(parts, 0);
    if (!json_object_object_get_ex(textObj, "text", &textField)) {
        registrarLog("ERRO: Campo 'text' ausente em 'parts[0]'");
        json_object_put(parsed_json);
        return false;
    }

    const char* fullText = json_object_get_string(textField);
    printf("[CHARADA LOG] Resposta completa da API: %s\n", fullText);

    // --- NOVO PARSING ROBUSTO ---
    // Ignorar prefixos/instruções e pegar só a primeira frase terminada em '?'
    const char* perguntaInicio = fullText;
    const char* perguntaFim = NULL;
    // Procurar a primeira interrogação
    perguntaFim = strchr(perguntaInicio, '?');
    if (perguntaFim) {
        // Voltar até o início da frase (pular prefixos e quebras de linha)
        // Avançar até a primeira letra após uma quebra de linha, se houver
        const char* fraseReal = perguntaInicio;
        // Procurar a última quebra de linha antes da interrogação
        const char* lastBreak = fraseReal;
        const char* temp = fraseReal;
        while (temp < perguntaFim) {
            if (*temp == '\n' || *temp == '\r') lastBreak = temp + 1;
            temp++;
        }
        if (lastBreak > fraseReal) fraseReal = lastBreak;
        // Copiar só a frase da pergunta
        size_t perguntaLen = perguntaFim - fraseReal + 1;
        strncpy(pergunta, fraseReal, perguntaLen);
        pergunta[perguntaLen] = '\0';
        // Agora pegar as alternativas normalmente (após o primeiro ' | ')
        char* altStart = strstr((char*)perguntaFim, " | ");
        if (altStart) {
            altStart += 3;
            char* delimiters = strstr(altStart, " | ");
            if (delimiters) {
                size_t resp1Len = delimiters - altStart;
                strncpy(resposta1, altStart, resp1Len);
                resposta1[resp1Len] = '\0';
                altStart = delimiters + 3;
                delimiters = strstr(altStart, " | ");
                if (delimiters) {
                    size_t resp2Len = delimiters - altStart;
                    strncpy(resposta2, altStart, resp2Len);
                    resposta2[resp2Len] = '\0';
                    strncpy(resposta3, delimiters + 3, sizeof(charadaAtual.alternativas[2]) - 1);
                    resposta3[sizeof(charadaAtual.alternativas[2]) - 1] = '\0';
                } else {
                    strncpy(resposta2, altStart, sizeof(charadaAtual.alternativas[1]) - 1);
                    resposta2[sizeof(charadaAtual.alternativas[1]) - 1] = '\0';
                    resposta3[0] = '\0';
                }
            } else {
                strncpy(resposta1, altStart, sizeof(charadaAtual.alternativas[0]) - 1);
                resposta1[sizeof(charadaAtual.alternativas[0]) - 1] = '\0';
                resposta2[0] = '\0';
                resposta3[0] = '\0';
            }
        } else {
            resposta1[0] = '\0'; resposta2[0] = '\0'; resposta3[0] = '\0';
        }
        json_object_put(parsed_json);
        return true;
    } else {
        // Fallback: não há '?', usar tudo antes do primeiro ' | '
        char* delimiters = strstr(fullText, " | ");
        if (delimiters) {
            size_t perguntaLen = delimiters - fullText;
            strncpy(pergunta, fullText, perguntaLen);
            pergunta[perguntaLen] = '\0';
            // Alternativas igual ao antigo
            char* resto = delimiters + 3;
            delimiters = strstr(resto, " | ");
            if (delimiters) {
                size_t resp1Len = delimiters - resto;
                strncpy(resposta1, resto, resp1Len);
                resposta1[resp1Len] = '\0';
                resto = delimiters + 3;
                delimiters = strstr(resto, " | ");
                if (delimiters) {
                    size_t resp2Len = delimiters - resto;
                    strncpy(resposta2, resto, resp2Len);
                    resposta2[resp2Len] = '\0';
                    strncpy(resposta3, delimiters + 3, sizeof(charadaAtual.alternativas[2]) - 1);
                    resposta3[sizeof(charadaAtual.alternativas[2]) - 1] = '\0';
                } else {
                    strncpy(resposta2, resto, sizeof(charadaAtual.alternativas[1]) - 1);
                    resposta2[sizeof(charadaAtual.alternativas[1]) - 1] = '\0';
                    resposta3[0] = '\0';
                }
            } else {
                strncpy(resposta1, resto, sizeof(charadaAtual.alternativas[0]) - 1);
                resposta1[sizeof(charadaAtual.alternativas[0]) - 1] = '\0';
                resposta2[0] = '\0';
                resposta3[0] = '\0';
            }
            json_object_put(parsed_json);
            return true;
        } else {
            registrarLog("ERRO: Formato inválido - resposta da API não contém os delimitadores esperados");
        }
    }
    json_object_put(parsed_json);
    return false;
}

// Função para gerar charada usando Gemini API
bool gerarCharadaGemini(int fase, char* pergunta, char* respostaCorreta, char* alternativa1, char* alternativa2) {
    CURL *curl;
    CURLcode res;
    ResponseData response;
    char prompt[512];
    char url[256];
    char postData[1024];
    
    registrarLog("Tentando gerar charada para a fase %d com a API Gemini", fase);
    
    response.data = malloc(1);
    if (!response.data) {
        registrarLog("ERRO CRÍTICO: Falha ao alocar memória para resposta da API");
        return false;
    }
    
    response.size = 0;
    
    // Criar o prompt adequado para cada fase
    switch (fase) {
        case 1:
            if (nivelDificuldade == FACIL) {
                sprintf(prompt, "Gere uma charada sombria e poética sobre obstáculos, com tom existencial, para um jogo chamado HEART. Formato: 'Pergunta? | resposta1 | resposta2 | resposta3'");
            } else if (nivelDificuldade == DIFICIL || nivelDificuldade == CUPHEAD) {
                sprintf(prompt, "Gere uma charada sombria, difícil e filosófica sobre obstáculos, com tom existencial, para um jogo chamado HEART. Formato: 'Pergunta? | resposta1 | resposta2 | resposta3'");
            } else {
                sprintf(prompt, "Gere uma charada sombria sobre obstáculos, com tom existencial, para um jogo chamado HEART. Formato: 'Pergunta? | resposta1 | resposta2 | resposta3'");
            }
            break;
        case 2:
            if (nivelDificuldade == FACIL) {
                sprintf(prompt, "Gere uma charada sombria e poética sobre labirintos, com tom existencial, para um jogo chamado HEART. Formato: 'Pergunta? | resposta1 | resposta2 | resposta3'");
            } else if (nivelDificuldade == DIFICIL || nivelDificuldade == CUPHEAD) {
                sprintf(prompt, "Gere uma charada sombria, difícil e filosófica sobre labirintos, com tom existencial, para um jogo chamado HEART. Formato: 'Pergunta? | resposta1 | resposta2 | resposta3'");
            } else {
                sprintf(prompt, "Gere uma charada sombria sobre labirintos, com tom existencial, para um jogo chamado HEART. Formato: 'Pergunta? | resposta1 | resposta2 | resposta3'");
            }
            break;
        case 3:
            if (nivelDificuldade == FACIL) {
                sprintf(prompt, "Gere uma charada sombria e poética sobre coração, sentimentos e memórias, com tom existencial, para um jogo chamado HEART. Formato: 'Pergunta? | resposta1 | resposta2 | resposta3'");
            } else if (nivelDificuldade == DIFICIL || nivelDificuldade == CUPHEAD) {
                sprintf(prompt, "Gere uma charada sombria, difícil e filosófica sobre coração, sentimentos e memórias, com tom existencial, para um jogo chamado HEART. Formato: 'Pergunta? | resposta1 | resposta2 | resposta3'");
            } else {
                sprintf(prompt, "Gere uma charada sombria sobre coração, sentimentos e memórias, com tom existencial, para um jogo chamado HEART. Formato: 'Pergunta? | resposta1 | resposta2 | resposta3'");
            }
            break;
        default:
            sprintf(prompt, "Gere uma charada sombria, existencial e poética para um jogo chamado HEART. Formato: 'Pergunta? | resposta1 | resposta2 | resposta3'");
    }
    
    registrarLog("Prompt: %s", prompt);
    
    // Formatar URL com a API key
    sprintf(url, "https://generativelanguage.googleapis.com/v1beta/models/gemini-2.0-flash:generateContent?key=%s", GEMINI_API_KEY);
    
    // Formatar dados da requisição
    sprintf(postData, 
        "{"
        "\"contents\": [{"
        "\"parts\":[{\"text\": \"%s\"}]"
        "}]"
        "}", 
        prompt
    );
    
    curl = curl_easy_init();
    if (curl) {
        struct curl_slist *headers = NULL;
        headers = curl_slist_append(headers, "Content-Type: application/json");
        
        curl_easy_setopt(curl, CURLOPT_URL, url);
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, postData);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&response);
        curl_easy_setopt(curl, CURLOPT_TIMEOUT, 10L); // Timeout de 10 segundos
        
        registrarLog("Enviando requisição à API Gemini...");
        res = curl_easy_perform(curl);
        
        // Verificar código de status HTTP
        long http_code = 0;
        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
        
        if (res != CURLE_OK) {
            registrarLog("ERRO: curl_easy_perform() falhou: %s", curl_easy_strerror(res));
            free(response.data);
            curl_slist_free_all(headers);
            curl_easy_cleanup(curl);
            return false;
        } else if (http_code != 200) {
            registrarLog("ERRO: API retornou código %ld", http_code);
            
            // Mostrar detalhes do erro
            if (response.size > 0) {
                char errorPreview[101] = {0};
                strncpy(errorPreview, response.data, 100);
                registrarLog("Detalhes do erro: %s", errorPreview);
            }
            
            free(response.data);
            curl_slist_free_all(headers);
            curl_easy_cleanup(curl);
            return false;
        }
        
        // Verificar se a resposta está vazia ou muito pequena
        if (response.size < 20) {
            registrarLog("ERRO: Resposta da API muito curta ou vazia (%zu bytes)", response.size);
            free(response.data);
            curl_slist_free_all(headers);
            curl_easy_cleanup(curl);
            return false;
        }
        
        // Mostrar os primeiros 100 caracteres da resposta para debug
        char previewResponse[101] = {0};
        strncpy(previewResponse, response.data, 100);
        registrarLog("Resposta recebida (preview): %s...", previewResponse);
        
        // Extrair dados da resposta
        bool resultado = extrairRespostaGemini(response.data, pergunta, respostaCorreta, alternativa1, alternativa2);
        
        if (resultado) {
            registrarLog("Charada gerada com sucesso pela API!");
            registrarLog("Pergunta: %s", pergunta);
            registrarLog("Resposta correta: %s", respostaCorreta);
            registrarLog("Alternativa 1: %s", alternativa1);
            registrarLog("Alternativa 2: %s", alternativa2);
        } else {
            registrarLog("ERRO: Falha ao extrair charada da resposta da API");
        }
        
        free(response.data);
        curl_slist_free_all(headers);
        curl_easy_cleanup(curl);
        
        return resultado;
    }
    
    registrarLog("ERRO: Falha ao inicializar CURL");
    free(response.data);
    return false;
}
#endif

// Inicializar sistema de charadas
void inicializarSistemaCharadas(void) {
    // Inicializar a lista encadeada de charadas
    inicializarListaCharadas(&listaCharadas);
    
    // Carregar textura de fundo com cor sólida (em vez de gradiente)
    Image img = GenImageColor(GetScreenWidth(), GetScreenHeight(), 
                            (Color){20, 10, 40, 255});
    fundoCharada = LoadTextureFromImage(img);
    UnloadImage(img);
    
    // Inicializar posição da caixa de input
    caixaInput = (Rectangle){
        GetScreenWidth()/2 - 200,
        GetScreenHeight()/2 + 50,
        400,
        40
    };
    
    // Inicializar sistema de logs
    sistemaLog.ativo = false;
    sistemaLog.currentLog = 0;
    for (int i = 0; i < MAX_LOGS; i++) {
        sistemaLog.logs[i].exibir = false;
    }
    
    // Inicializar CURL para requisições HTTP
#if HAS_CURL
    curl_global_init(CURL_GLOBAL_ALL);
#endif
    
    // Inicializar primeira charada
    memset(&charadaAtual, 0, sizeof(Charada));
}

// Liberar recursos
void liberarRecursosCharada(void) {
    // Liberar memória da lista encadeada de charadas
    liberarListaCharadas(&listaCharadas);
    
    UnloadTexture(fundoCharada);
#if HAS_CURL
    curl_global_cleanup();
#endif
}

// Verificar se a resposta está correta
bool verificarResposta(void) {
    // Converter tentativa para minúsculas
    char tentativaLower[128] = {0};
    strcpy(tentativaLower, charadaAtual.tentativaAtual);
    for(int i = 0; tentativaLower[i]; i++) {
        tentativaLower[i] = tolower(tentativaLower[i]);
    }
    
    // Verificar se contém a palavra-chave da resposta
    if(strstr(tentativaLower, charadaAtual.resposta)) {
        charadaAtual.acertou = true;
        charadaAtual.respondida = true;
        strcpy(mensagemStatus, "CORRETO! Avançando para a próxima fase...");
        tempoFeedback = 2.0f;
        return true;
    } else {
        charadaAtual.tentativasRestantes--;
        if(charadaAtual.tentativasRestantes <= 0) {
            charadaAtual.respondida = true;
            strcpy(mensagemStatus, "GAME OVER! Você esgotou suas tentativas.");
            tempoFeedback = 2.0f;
        } else {
            sprintf(mensagemStatus, "Incorreto! Tentativas restantes: %d", charadaAtual.tentativasRestantes);
            tempoFeedback = 2.0f;
        }
        return false;
    }
}

// Atualizar a tela de charada
bool atualizarTelaCharada(void) {
    if (!modoCharada) return false;

    // Atualizar temporizador de feedback
    if (tempoFeedback > 0) {
        tempoFeedback -= GetFrameTime();
        if (tempoFeedback <= 0 && charadaAtual.respondida) {
            if (charadaAtual.acertou) {
                modoCharada = false;
                // Avançar para a próxima fase
                if (charadaAtual.faseAlvo == 2) {
                    faseAtual = 2;
                    mudarParaFase2();
                } else if (charadaAtual.faseAlvo == 3) {
                    faseAtual = 3;
                    mudarParaFase3();
                } else if (charadaAtual.faseAlvo == 4) {
                    // Vitória do jogo
                    return false; // Jogo vencido
                }
                return true; // Continua o jogo
            } else if (charadaAtual.tentativasRestantes <= 0) {
                modoCharada = false;
                return false; // Game Over
            }
        }
    }

    // Processar cliques nos botões de alternativas
    if (!charadaAtual.respondida) {
        int buttonWidth = 400;
        int buttonHeight = 50;
        int buttonY = GetScreenHeight() / 2 + 20;

        // Verificar clique do mouse apenas uma vez por frame
        bool mouseClicked = IsMouseButtonReleased(MOUSE_LEFT_BUTTON);
        if (mouseClicked) {
            Vector2 mousePos = GetMousePosition();
            
            // Verificar cada botão
            for (int i = 0; i < 3; i++) {
                int buttonX = GetScreenWidth() / 2 - buttonWidth / 2;
                Rectangle buttonRect = {buttonX, buttonY + i * (buttonHeight + 10), buttonWidth, buttonHeight};
                
                if (CheckCollisionPointRec(mousePos, buttonRect)) {
                    registrarLog("Botão %d clicado: %s", i + 1, charadaAtual.alternativas[i]);
                    // Verificar se a resposta está correta
                    if (strcmp(charadaAtual.alternativas[i], charadaAtual.resposta) == 0) {
                        charadaAtual.acertou = true;
                        strcpy(mensagemStatus, "CORRETO! Avançando para a próxima fase...");
                    } else {
                        charadaAtual.tentativasRestantes--;
                        if (charadaAtual.tentativasRestantes <= 0) {
                            strcpy(mensagemStatus, "GAME OVER! Você esgotou suas tentativas.");
                        } else {
                            sprintf(mensagemStatus, "Incorreto! Tentativas restantes: %d", charadaAtual.tentativasRestantes);
                        }
                    }
                    
                    // Garantir que o estado de respondida seja atualizado corretamente
                    charadaAtual.respondida = (charadaAtual.tentativasRestantes <= 0 || charadaAtual.acertou);
                    tempoFeedback = 2.0f;
                    break;
                }
            }
        }
    }

    // Escape para voltar ao menu (opcional)
    if (IsKeyPressed(KEY_ESCAPE)) {
        modoCharada = false;
        return false;
    }

    return true; // Continuando no modo charada
}

// Desenhar a tela de charada
void desenharTelaCharada(void) {
    if (!modoCharada) return;

    // Desenhar fundo
    DrawTexture(fundoCharada, 0, 0, WHITE);

    // Desenhar título
    char titulo[64];
    sprintf(titulo, "CHARADA DA FASE %d", charadaAtual.faseAlvo);
    int tituloWidth = MeasureText(titulo, 40);
    DrawText(titulo, GetScreenWidth() / 2 - tituloWidth / 2, 60, 40, GOLD);

    // --- FRONTEND ROBUSTO: PERGUNTA EM CAIXA DESTACADA COM WORD WRAP REAL ---
    int fontSize = 28;
    int questionBoxWidth = 700;
    int maxBoxHeight = 340; // Altura máxima permitida
    int minBoxHeight = 80;  // Altura mínima
    int questionBoxX = GetScreenWidth() / 2 - questionBoxWidth / 2;
    int tituloY = 60;
    int tituloHeight = 40;
    int minTopMargin = 40; // margem mínima entre título e caixa
    int questionBoxY = tituloY + tituloHeight + minTopMargin;
    int lineSpacing = 8;

    // Calcular altura necessária para o texto da pergunta
    int textLen = strlen(charadaAtual.pergunta);
    int numLines = 1;
    int lineStart = 0;
    for (int i = 0; i < textLen; ) {
        int bestBreak = -1;
        int j = i;
        while (j < textLen && charadaAtual.pergunta[j] != '\0') {
            if (charadaAtual.pergunta[j] == ' ' || charadaAtual.pergunta[j] == '\n') bestBreak = j;
            int len = j - lineStart + 1;
            char temp[512];
            strncpy(temp, &charadaAtual.pergunta[lineStart], len);
            temp[len] = '\0';
            int w = MeasureText(temp, fontSize);
            if (w > questionBoxWidth - 40) break;
            if (charadaAtual.pergunta[j] == '\n') break;
            j++;
        }
        if (bestBreak > lineStart) {
            i = bestBreak + 1;
        } else if (j > lineStart) {
            i = j;
        } else {
            i++;
        }
        lineStart = i;
        if (i < textLen) numLines++;
    }
    int questionBoxHeight = minBoxHeight + (numLines - 1) * (fontSize + lineSpacing);
    if (questionBoxHeight > maxBoxHeight) questionBoxHeight = maxBoxHeight;

    // Centralizar a caixa verticalmente no espaço útil (entre título e alternativas)
    int altBoxWidth = 500;
    int altBoxHeight = 60;
    int altSpacing = 30;
    int totalHeight = questionBoxHeight + 3 * (altBoxHeight + altSpacing) + 40 + 40; // caixa + alternativas + margens
    int availableHeight = GetScreenHeight() - (tituloY + tituloHeight) - 40; // 40px margem inferior
    int offsetY = (availableHeight - totalHeight) / 2;
    if (offsetY < 0) offsetY = 0;
    questionBoxY = tituloY + tituloHeight + minTopMargin + offsetY;
    int altStartY = questionBoxY + questionBoxHeight + 40;

    DrawRectangleRounded((Rectangle){questionBoxX, questionBoxY, questionBoxWidth, questionBoxHeight}, 0.25f, 16, (Color){30, 20, 60, 240});
    DrawRectangleRoundedLines((Rectangle){questionBoxX, questionBoxY, questionBoxWidth, questionBoxHeight}, 0.25f, 16, GOLD);

    // Pergunta centralizada e em destaque (apenas a mensagem da IA)
    Rectangle perguntaRect = {questionBoxX, questionBoxY, questionBoxWidth, questionBoxHeight};
    DrawTextBoxedCentered(charadaAtual.pergunta, perguntaRect, fontSize, 1, WHITE);

    // --- ALTERNATIVAS EM CAIXAS SEPARADAS, AJUSTADAS ---
    int altBoxX = GetScreenWidth() / 2 - altBoxWidth / 2;
    for (int i = 0; i < 3; i++) {
        int y = altStartY + i * (altBoxHeight + altSpacing);
        Rectangle altRect = {altBoxX, y, altBoxWidth, altBoxHeight};
        Color boxColor = (Color){50, 30, 80, 220};
        Color borderColor = WHITE;
        Vector2 mousePos = GetMousePosition();
        bool isHovering = CheckCollisionPointRec(mousePos, altRect);
        if (isHovering) {
            boxColor = (Color){80, 60, 120, 255};
            borderColor = GOLD;
        }
        DrawRectangleRounded(altRect, 0.18f, 12, boxColor);
        DrawRectangleRoundedLines(altRect, 0.18f, 12, borderColor);
        int altTextWidth = MeasureText(charadaAtual.alternativas[i], 24);
        DrawText(charadaAtual.alternativas[i], altBoxX + (altBoxWidth - altTextWidth) / 2, y + 16, 24, WHITE);
    }

    // Instruções
    int textWidth = MeasureText("Escolha a resposta correta clicando em uma alternativa", 20);
    DrawText("Escolha a resposta correta clicando em uma alternativa", GetScreenWidth() / 2 - textWidth / 2, altStartY + 3 * (altBoxHeight + altSpacing) + 10, 20, LIGHTGRAY);

    // Mensagem de status
    if (tempoFeedback > 0) {
        Color statusColor = WHITE;
        if (strstr(mensagemStatus, "CORRETO")) statusColor = GREEN;
        else if (strstr(mensagemStatus, "GAME OVER")) statusColor = RED;
        else if (strstr(mensagemStatus, "Incorreto")) statusColor = ORANGE;
        int statusWidth = MeasureText(mensagemStatus, 30);
        DrawText(mensagemStatus, GetScreenWidth() / 2 - statusWidth / 2, altStartY + 3 * (altBoxHeight + altSpacing) + 50, 30, statusColor);
    }

    // Tentativas restantes
    char tentativas[32];
    sprintf(tentativas, "Tentativas: %d", charadaAtual.tentativasRestantes);
    textWidth = MeasureText(tentativas, 20);
    DrawText(tentativas, GetScreenWidth() - textWidth - 20, 20, 20, LIME);

    // Logs de charadas (mantém igual, mas agora abaixo das alternativas para não sobrepor a pergunta)
    int logCount = 0;
    int logsStartY = altStartY + 3 * (altBoxHeight + altSpacing) + 90; // abaixo das alternativas e mensagens
    for (int i = 0; i < MAX_LOGS; i++) {
        if (sistemaLog.logs[i].exibir) {
            int logY = logsStartY + logCount * 35;
            Color logColor;
            switch (sistemaLog.logs[i].tipo) {
                case LOG_WARN:  logColor = YELLOW; break;
                case LOG_ERROR: logColor = RED; break;
                default:        logColor = WHITE; break;
            }
            int logWidth = MeasureText(sistemaLog.logs[i].mensagem, 18) + 20;
            if (logWidth > GetScreenWidth() - 40) logWidth = GetScreenWidth() - 40;
            DrawRectangle(10, logY - 5, logWidth, 30, (Color){0, 0, 0, 180});
            DrawText(sistemaLog.logs[i].mensagem, 20, logY, 18, logColor);
            logCount++;
        }
    }
    // Atualização dos logs e sistemaLog.ativo permanece igual
}

// Função para gerar charada usando Gemini API
void gerarNovaCharada(int fase) {
    // Limitação de segurança
    if (fase < 1 || fase > 3) fase = 1;

    // Inicializar estado da charada
    static int ultimaCharada[3] = {-1, -1, -1}; // Último índice usado para cada fase

    charadaAtual.tentativasRestantes = 3;
    charadaAtual.ativa = true;
    charadaAtual.respondida = false;
    charadaAtual.acertou = false;
    charadaAtual.faseAlvo = fase;
    charadaAtual.tentativaAtual[0] = '\0';
    
    // Verificar se já temos charadas na lista para esta fase
    bool charadaEncontrada = false;
    int tamanhoLista = 0;
    
    // Contamos o número de charadas na lista
    NodoCharada* atual = listaCharadas.inicio;
    while (atual != NULL) {
        if (atual->charada.faseAlvo == fase) {
            tamanhoLista++;
        }
        atual = atual->proximo;
    }
    
    // Se temos pelo menos uma charada armazenada para esta fase, temos 50% de chance de reutilizá-la
    if (tamanhoLista > 0 && GetRandomValue(0, 1) == 0) {
        // Selecionar uma charada aleatória da lista
        int indiceAleatorio = GetRandomValue(0, tamanhoLista - 1);
        int contador = 0;
        
        atual = listaCharadas.inicio;
        while (atual != NULL) {
            if (atual->charada.faseAlvo == fase) {
                if (contador == indiceAleatorio) {
                    // Encontramos a charada, copiar para charadaAtual
                    charadaAtual = atual->charada;
                    charadaAtual.tentativasRestantes = 3; // Resetar tentativas
                    charadaAtual.respondida = false;
                    charadaAtual.acertou = false;
                    charadaAtual.tentativaAtual[0] = '\0';
                    charadaEncontrada = true;
                    break;
                }
                contador++;
            }
            atual = atual->proximo;
        }
    }
    
    // Se não encontramos ou decidimos não reutilizar, geramos uma nova
    if (!charadaEncontrada) {

#if HAS_CURL
    // Tentar gerar uma charada usando a API Gemini
    char pergunta[512] = {0};
    char resposta1[64] = {0};
    char resposta2[64] = {0};
    char resposta3[64] = {0};
    
    if (gerarCharadaGemini(fase, pergunta, resposta1, resposta2, resposta3)) {
        // Usar a charada gerada pela API
        strncpy(charadaAtual.pergunta, pergunta, sizeof(charadaAtual.pergunta) - 1);
        charadaAtual.pergunta[sizeof(charadaAtual.pergunta) - 1] = '\0';
        
        strncpy(charadaAtual.resposta, resposta1, sizeof(charadaAtual.resposta) - 1);
        charadaAtual.resposta[sizeof(charadaAtual.resposta) - 1] = '\0';
        
        // Armazenar alternativas
        strncpy(charadaAtual.alternativas[0], resposta1, sizeof(charadaAtual.alternativas[0]) - 1);
        charadaAtual.alternativas[0][sizeof(charadaAtual.alternativas[0]) - 1] = '\0';
        
        strncpy(charadaAtual.alternativas[1], resposta2, sizeof(charadaAtual.alternativas[1]) - 1);
        charadaAtual.alternativas[1][sizeof(charadaAtual.alternativas[1]) - 1] = '\0';
        
        strncpy(charadaAtual.alternativas[2], resposta3, sizeof(charadaAtual.alternativas[2]) - 1);
        charadaAtual.alternativas[2][sizeof(charadaAtual.alternativas[2]) - 1] = '\0';
        
        // Embaralhar alternativas
        for (int i = 0; i < 3; i++) {
            int j = GetRandomValue(0, 2);
            char temp[64];
            strncpy(temp, charadaAtual.alternativas[i], sizeof(temp) - 1);
            temp[sizeof(temp) - 1] = '\0';
            strncpy(charadaAtual.alternativas[i], charadaAtual.alternativas[j], sizeof(charadaAtual.alternativas[i]) - 1);
            charadaAtual.alternativas[i][sizeof(charadaAtual.alternativas[i]) - 1] = '\0';
            strncpy(charadaAtual.alternativas[j], temp, sizeof(charadaAtual.alternativas[j]) - 1);
            charadaAtual.alternativas[j][sizeof(charadaAtual.alternativas[j]) - 1] = '\0';
        }
        
        return;
    } else {
        registrarLog("AVISO: Usando charadas pré-definidas como fallback");
    }
    
    // Se falhou em gerar com API, usar as charadas pré-definidas
#endif

    // Charadas pré-definidas com múltiplas respostas
    const char* charadasFase1[][4] = {
        {"Sou veloz como o vento, mas não tenho corpo. Posso te machucar, mas nunca te tocar. O que sou eu?", "obstáculo", "vento", "fogo"},
        {"Em um mundo digital, sou a luz que se move. Vermelho como sangue, mas não estou vivo. O que sou eu?", "coração", "sangue", "luz"},
        {"Quanto mais me moverem, mais forte fico. Quando me tocam, causo dor. O que sou eu?", "osso", "pedra", "metal"}
    };

    const char* charadasFase2[][4] = {
        {"Estou sempre correndo, mas nunca chego a lugar nenhum. O que sou eu?", "jogo", "tempo", "vento"},
        {"Sou invisível até te ferir. Depois disso, não posso mais te machucar. O que sou eu?", "armadilha", "sombra", "vento"},
        {"Em um labirinto vermelho, me escondo nas sombras. Quando me encontrarem, já será tarde demais. O que sou eu?", "perigo", "medo", "sombra"}
    };

    const char* charadasFase3[][4] = {
        {"Quanto mais rápido você corre, mais eu me aproximo. O que sou eu?", "destino", "tempo", "sombra"},
        {"Sou feito de memórias e pesadelos. Toda vez que me enfrenta, fico mais forte. O que sou eu?", "medo", "sombra", "dor"},
        {"No centro de tudo, pulsando sem parar, sou seu maior aliado e seu pior inimigo. O que sou eu?", "coração", "vida", "tempo"}
    };

    const char* (*charadas)[4];
    int numCharadas;

    switch (fase) {
        case 1:
            charadas = charadasFase1;
            numCharadas = 3;
            break;
        case 2:
            charadas = charadasFase2;
            numCharadas = 3;
            break;
        case 3:
        default:
            charadas = charadasFase3;
            numCharadas = 3;
            break;
    }

    // Escolher charada aleatória
    int indice = GetRandomValue(0, numCharadas - 1);
    
    // Evitar repetição, se possível
    if (numCharadas > 1) {
        int tentativas = 0;
        while (indice == ultimaCharada[fase - 1] && tentativas < 3) {
            indice = GetRandomValue(0, numCharadas - 1);
            tentativas++;
        }
    }

    ultimaCharada[fase - 1] = indice;

    // Garante que o índice está dentro dos limites
    if (indice < 0 || indice >= numCharadas) indice = 0;

    // Copia segura das strings para evitar problemas de memória
    strncpy(charadaAtual.pergunta, charadas[indice][0], sizeof(charadaAtual.pergunta) - 1);
    charadaAtual.pergunta[sizeof(charadaAtual.pergunta) - 1] = '\0';
    
    strncpy(charadaAtual.resposta, charadas[indice][1], sizeof(charadaAtual.resposta) - 1);
    charadaAtual.resposta[sizeof(charadaAtual.resposta) - 1] = '\0';

    // Armazenar alternativas
    strncpy(charadaAtual.alternativas[0], charadas[indice][1], sizeof(charadaAtual.alternativas[0]) - 1); 
    charadaAtual.alternativas[0][sizeof(charadaAtual.alternativas[0]) - 1] = '\0';
    
    strncpy(charadaAtual.alternativas[1], charadas[indice][2], sizeof(charadaAtual.alternativas[1]) - 1);
    charadaAtual.alternativas[1][sizeof(charadaAtual.alternativas[1]) - 1] = '\0';
    
    strncpy(charadaAtual.alternativas[2], charadas[indice][3], sizeof(charadaAtual.alternativas[2]) - 1);
    charadaAtual.alternativas[2][sizeof(charadaAtual.alternativas[2]) - 1] = '\0';    // Embaralhar alternativas
    for (int i = 0; i < 3; i++) {
        int j = GetRandomValue(0, 2);
        char temp[64] = {0};
        strncpy(temp, charadaAtual.alternativas[i], sizeof(temp) - 1);
        strncpy(charadaAtual.alternativas[i], charadaAtual.alternativas[j], sizeof(charadaAtual.alternativas[i]) - 1);
        charadaAtual.alternativas[i][sizeof(charadaAtual.alternativas[i]) - 1] = '\0';
        strncpy(charadaAtual.alternativas[j], temp, sizeof(charadaAtual.alternativas[j]) - 1);
        charadaAtual.alternativas[j][sizeof(charadaAtual.alternativas[j]) - 1] = '\0';
    }
    
    // Adicionar a charada à lista encadeada se ela foi gerada pela primeira vez
    if (!charadaEncontrada) {
        inserirCharada(&listaCharadas, charadaAtual);
        registrarLog("INFO: Nova charada adicionada à lista para a fase %d", fase);
    }
}

// Implementação da função registrarLog
void registrarLog(const char* formato, ...) {
    va_list args;
    va_start(args, formato);
    
    char buffer[1024];
    vsnprintf(buffer, sizeof(buffer), formato, args);
    
#if DEBUG_LOGS
    // Configurar o tipo de log com base no conteúdo da mensagem
    int tipoLog = 0; // LOG_INFO padrão
    if (strstr(buffer, "ERRO")) {
        tipoLog = 1; // LOG_ERROR
    } else if (strstr(buffer, "AVISO")) {
        tipoLog = 2; // LOG_WARN
    }
    
    // Imprimir a mensagem de log
    printf("[%s] %s\n", 
           (tipoLog == 1) ? "ERRO" : (tipoLog == 2) ? "AVISO" : "INFO",
           buffer);
    
    // Adicionar ao sistema de logs (buscar um slot livre)
    int slotLivre = -1;
    for (int i = 0; i < MAX_LOGS; i++) {
        if (!sistemaLog.logs[i].exibir) {
            slotLivre = i;
            break;
        }
    }
    
    // Se não encontrou slot livre, substitui o log mais antigo
    if (slotLivre == -1) {
        slotLivre = sistemaLog.currentLog;
        sistemaLog.currentLog = (sistemaLog.currentLog + 1) % MAX_LOGS;
    }
    
    // Guardar mensagem no log
    strncpy(sistemaLog.logs[slotLivre].mensagem, buffer, sizeof(sistemaLog.logs[slotLivre].mensagem) - 1);
    sistemaLog.logs[slotLivre].mensagem[sizeof(sistemaLog.logs[slotLivre].mensagem) - 1] = '\0';
    sistemaLog.logs[slotLivre].exibir = true;
    sistemaLog.logs[slotLivre].tempo = 5.0f; // Exibir por 5 segundos
    sistemaLog.logs[slotLivre].posY = 150;
    sistemaLog.logs[slotLivre].tipo = tipoLog;
    sistemaLog.ativo = true;
    
    // Também registrar no console para debug
    printf("[CHARADA LOG] %s\n", buffer);
#endif
    va_end(args);
}


void processarRespostaAPI(const char* resposta) {
    struct json_object *parsed_json, *candidates, *content, *parts, *text;
    parsed_json = json_tokener_parse(resposta);

    if (!parsed_json) {
        registrarLog("ERRO: Falha ao parsear a resposta JSON da API");
        registrarLog("Resposta completa: %s", resposta);
        goto fallback;
    }

    if (!json_object_object_get_ex(parsed_json, "candidates", &candidates)) {
        registrarLog("ERRO: Campo 'candidates' ausente na resposta JSON");
        registrarLog("Resposta completa: %s", resposta);
        goto fallback;
    }

    int num_candidates = json_object_array_length(candidates);
    if (num_candidates == 0) {
        registrarLog("ERRO: Nenhum candidato encontrado na resposta JSON");
        registrarLog("Resposta completa: %s", resposta);
        goto fallback;
    }

    for (int i = 0; i < num_candidates; i++) {
        content = json_object_array_get_idx(candidates, i);
        if (!json_object_object_get_ex(content, "content", &content)) {
            registrarLog("ERRO: Campo 'content' ausente em 'candidates[%d]'", i);
            continue;
        }

        if (!json_object_object_get_ex(content, "parts", &parts)) {
            registrarLog("ERRO: Campo 'parts' ausente em 'content'");
            continue;
        }

        int num_parts = json_object_array_length(parts);
        for (int j = 0; j < num_parts; j++) {
            text = json_object_array_get_idx(parts, j);
            if (json_object_object_get_ex(text, "text", &text)) {
                registrarLog("Mensagem recebida: %s", json_object_get_string(text));
            } else {
                registrarLog("ERRO: Campo 'text' ausente em 'parts[%d]'", j);
            }
        }
    }

    json_object_put(parsed_json); // Liberar memória
    return;

fallback:
    registrarLog("[CHARADA LOG] AVISO: Usando charadas pré-definidas como fallback");
    registrarLog("Charada fallback: Qual é o caminho mais curto em um labirinto? | resposta1 | resposta2 | resposta3");
    if (parsed_json) json_object_put(parsed_json); // Liberar memória
}
}

// Função auxiliar para desenhar texto com word wrap centralizado em um retângulo
void DrawTextBoxedCentered(const char *text, Rectangle rec, int fontSize, int spacing, Color color) {
    int maxWidth = (int)rec.width - 40; // margem interna
    char linha[512] = "";
    int y = 0;
    const char *p = text;
    int numLines = 0;
    // Primeiro, contar linhas para centralizar verticalmente
    while (*p) {
        int len = 0, lastSpace = -1;
        int width = 0;
        while (p[len] && p[len] != '\n') {
            char c = p[len];
            if (c == ' ') lastSpace = len;
            int charWidth = MeasureTextEx(GetFontDefault(), &c, fontSize, spacing).x;
            width += charWidth;
            if (width > maxWidth) break;
            if (p[len] == '\n') break;
            len++;
        }
        if (width > maxWidth && lastSpace > 0) len = lastSpace;
        if (len == 0) len = 1;
        numLines++;
        p += len;
        while (*p == ' ') p++;
        if (*p == '\n') p++;
    }
    // Agora desenhar centralizado verticalmente
    p = text;
    y = (int)(rec.y + (rec.height - numLines * fontSize) / 2);
    while (*p) {
        int len = 0, lastSpace = -1;
        int width = 0;
        while (p[len] && p[len] != '\n') {
            char c = p[len];
            if (c == ' ') lastSpace = len;
            int charWidth = MeasureTextEx(GetFontDefault(), &c, fontSize, spacing).x;
            width += charWidth;
            if (width > maxWidth) break;
            len++;
        }
        if (width > maxWidth && lastSpace > 0) len = lastSpace;
        if (len == 0) len = 1;
        strncpy(linha, p, len);
        linha[len] = '\0';
        // Remover espaços à direita
        int end = len - 1;
        while (end >= 0 && linha[end] == ' ') { linha[end] = '\0'; end--; }
        int lineWidth = MeasureText(linha, fontSize);
        DrawText(linha, (int)(rec.x + (rec.width - lineWidth) / 2), y, fontSize, color);
        y += fontSize;
        p += len;
        while (*p == ' ') p++;
        if (*p == '\n') p++;
    }
}
