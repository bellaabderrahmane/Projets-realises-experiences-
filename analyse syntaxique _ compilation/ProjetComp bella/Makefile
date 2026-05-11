CC = gcc
CFLAGS = -Wall -g -Isrc
LDFLAGS = -lfl
PARSER = le_parser
LEXER = le_lexer

# Cibles de l'exécutable
bin/tpcc: obj/$(LEXER).o obj/$(PARSER).o obj/tree.o obj/symbole.o | dirs
	$(CC) -o $@ $^ $(LDFLAGS)
	echo "Compilation terminée avec succès."

# Cibles des objets
obj/tree.o: src/tree.c src/tree.h | dirs
	$(CC) -c -o $@ $< $(CFLAGS)

obj/symbole.o: src/symbole.c src/symbole.h | dirs
	$(CC) -c -o $@ $< $(CFLAGS)

obj/$(PARSER).o: obj/$(PARSER).c | dirs
	$(CC) -c -o $@ $< $(CFLAGS)

obj/$(LEXER).o: obj/$(LEXER).c obj/$(PARSER).h | dirs
	$(CC) -c  -o $@ $< $(CFLAGS)

# Génération des fichiers par flex et bison
obj/$(LEXER).c: src/$(LEXER).l obj/$(PARSER).h | dirs
	flex -o $@ $<

obj/$(PARSER).c obj/$(PARSER).h: src/$(PARSER).y | dirs
	bison -d -Wcounterexamples -o obj/$(PARSER).c $<

# Commande pour créer les répertoires bin et obj si nécessaire
dirs:
	mkdir -p bin obj

# Règle par défaut
all: bin/tpcc

# Règle pour exécuter le compilateur avec un fichier d'entrée
run: bin/tpcc
	./bin/tpcc < test.tpc

# Commande pour nettoyer les fichiers générés
clean:
	rm -f bin/tpcc obj/*.o obj/$(LEXER).c obj/$(PARSER).c obj/$(PARSER).h
	rm -rf bin obj
