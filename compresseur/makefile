
CC = gcc
CFLAGS = -Wall -Icomrpessor/header
LDFLAGS = -lm



SRC_DIR = comrpessor/source
HEADER_DIR = comrpessor/header
MAIN_DIR = comrpessor/main
EXEC_DIR = comrpessor/executable

SRC_FILES = $(wildcard $(SRC_DIR)/*.c)
OBJ_FILES = $(SRC_FILES:.c=.o)
MAIN_FILE = $(MAIN_DIR)/main.c
EXECUTABLE = $(EXEC_DIR)/compressor

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJ_FILES) $(MAIN_FILE)
	$(CC) $(CFLAGS) $(OBJ_FILES) $(MAIN_FILE) $(LDFLAGS) -o $(EXECUTABLE)

$(SRC_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(SRC_DIR)/*.o $(EXEC_DIR)/compressor

.PHONY: all clean
