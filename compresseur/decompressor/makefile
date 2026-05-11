
CC = gcc
CFLAGS = -Wall -Wextra -O2 -Iheader -std=c99
LDFLAGS = -lm

SRCDIR = source
HEADERDIR = header
MAINDIR = main
EXECDIR = executable

SOURCES = $(wildcard $(SRCDIR)/*.c) $(MAINDIR)/main.c
OBJECTS = $(SOURCES:.c=.o)
EXECUTABLE = $(EXECDIR)/decompressor

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(CFLAGS)  -o $@ $^ $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(SRCDIR)/*.o $(MAINDIR)/*.o
	rm -f $(EXECUTABLE)

.PHONY: all clean
