# Variables
CC = gcc
CFLAGS = -I include -lm -lMLV
DEPS = include/main.h include/graphics.h include/gem.h include/tower.h include/wave.h include/monster.h include/terrain.h
OBJ = src/main.o src/graphics.o src/gem.o src/tower.o src/wave.o src/monster.o src/terrain.o
BIN = bin/game

# Default make
all: $(BIN)

# Linking the final executable
$(BIN): $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

# Compiling source files
src/%.o: src/%.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

# Clean up
.PHONY: clean
clean:
	rm -f $(OBJ) $(BIN)
