#ui: ui.c
#	gcc `pkg-config --cflags gtk+-3.0` -o ui ui.c `pkg-config --libs gtk+-3.0`

#clean:
#	rm ui

CC = gcc
CFLAGS = -Wall -Wextra -g -fsanitize=address
GTK_CFLAGS = `pkg-config --cflags gtk+-3.0`
GTK_LIBS = `pkg-config --libs gtk+-3.0`
MATH_LIB = -lm  

INCLUDES = -I. -I../preporcessor/src -I../stb -I..

# CORRECTION : AJOUT de ../preporcessor/src/process.c
PREP_SOURCES = ../preporcessor/src/filters.c ../preporcessor/src/main.c ../preporcessor/src/utils.c ../preporcessor/src/process.c
PREP_OBJECTS = $(patsubst %.c, %.o, $(PREP_SOURCES))

SOLVER_SOURCES = ../solver.c
SOLVER_OBJECTS = $(patsubst %.c, %.o, $(SOLVER_SOURCES))


ui: ui.c $(PREP_OBJECTS) $(SOLVER_OBJECTS)
	$(CC) $(CFLAGS) $(GTK_CFLAGS) $(INCLUDES) -o $@ $^ $(MATH_LIB) $(GTK_LIBS)

# Règle pour compiler les fichiers C du preprocesseur en fichiers objets (.o)
../preporcessor/src/%.o: ../preporcessor/src/%.c
	$(CC) $(CFLAGS) $(GTK_CFLAGS) $(INCLUDES) -c $< -o $@

# Règle pour compiler le solver
../%.o: ../%.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

.PHONY: clean

clean:
	rm -f ui
	rm -f $(PREP_OBJECTS)
	rm -f $(SOLVER_OBJECTS)
