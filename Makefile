CC = gcc
EXEC = tetris
SRC_LIST = src/main.c src/game.c src/engine.c

all: $(EXEC)

$(EXEC): $(SRC_LIST)
	$(CC) -o $@ $(SRC_LIST) -lncursesw -lm

