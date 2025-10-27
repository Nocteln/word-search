#ui: ui.c
#	gcc `pkg-config --cflags gtk+-3.0` -o ui ui.c `pkg-config --libs gtk+-3.0`

#clean:
#	rm ui

CC = gcc
CFLAGS = -Wall -Wextra -g
GTK_CFLAGS = `pkg-config --cflags gtk+-3.0`
GTK_LIBS = `pkg-config --libs gtk+-3.0`
MATH_LIB = -lm  

INCLUDES = -I. -I../a/preporcessor/src -I../a/stb

# CORRECTION : AJOUT de ../a/preporcessor/src/process.c
PREP_SOURCES = ../a/preporcessor/src/filters.c ../a/preporcessor/src/main.c ../a/preporcessor/src/utils.c ../a/preporcessor/src/process.c
PREP_OBJECTS = $(patsubst %.c, %.o, $(PREP_SOURCES))


ui: ui.c $(PREP_OBJECTS)
	$(CC) $(CFLAGS) $(GTK_CFLAGS) $(INCLUDES) -o $@ $^ $(MATH_LIB) $(GTK_LIBS)

# Règle pour compiler les fichiers C du preprocesseur en fichiers objets (.o)
../a/preporcessor/src/%.o: ../a/preporcessor/src/%.c
	$(CC) $(CFLAGS) $(GTK_CFLAGS) $(INCLUDES) -c $< -o $@

.PHONY: clean

clean:
	rm -f ui
	rm -f $(PREP_OBJECTS)
