# Nome do executável
TARGET = heart

# Fontes do jogo (atualizadas para incluir os novos arquivos)
SRCS = main.c menu.c jogo.c pontuacao.c boss.c ia_gemini.c gemini_config.c

# Detectar sistema operacional
ifeq ($(OS),Windows_NT)
    # Bibliotecas para Windows (adicionada curl para API Gemini)
    LIBS = -lraylib -lopengl32 -lgdi32 -lwinmm -lcurl
    # Comando para limpar
    RM = del
    # Extensão do executável
    EXT = .exe
else
    # Bibliotecas para Linux (adicionada curl para API Gemini)
    LIBS = -lraylib -lGL -lm -lpthread -ldl -lrt -lX11 -lcurl
    # Comando para limpar
    RM = rm -f
    # Extensão do executável
    EXT =
endif

# Compilador
CC = gcc

# Flags do compilador
CFLAGS = -Wall -std=c99

# Regra padrão
all:
	$(CC) $(CFLAGS) $(SRCS) -o $(TARGET)$(EXT) $(LIBS)

# Limpeza de arquivos compilados
clean:
	$(RM) $(TARGET)$(EXT)

# Executar o jogo
run: all
	./$(TARGET)$(EXT)
