# Nome do executável
TARGET = heart

# Fontes do jogo
SRCS = game.c menu.c

# Bibliotecas a linkar
LIBS = -lraylib -lGL -lm -lpthread -ldl -lrt -lX11

# Compilador
CC = gcc

# Regra padrão
all:
	$(CC) $(SRCS) -o $(TARGET) $(LIBS)

# Limpeza de arquivos compilados
clean:
	rm -f $(TARGET)
