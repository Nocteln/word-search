#ui: ui.c
#	gcc `pkg-config --cflags gtk+-3.0` -o ui ui.c `pkg-config --libs gtk+-3.0`

#clean:
#	rm ui

CC = gcc
CFLAGS = -Wall -Wextra -g -fsanitize=address
GTK_CFLAGS = `pkg-config --cflags gtk+-3.0`
GTK_LIBS = `pkg-config --libs gtk+-3.0`
MATH_LIB = -lm  

INCLUDES = -I. -I../preporcessor/src -I../neural_network/src -I../stb -I..

# CORRECTION : AJOUT de ../preporcessor/src/process.c
PREP_SOURCES = ../preporcessor/src/filters.c ../preporcessor/src/main.c ../preporcessor/src/utils.c ../preporcessor/src/process.c
PREP_OBJECTS = $(patsubst %.c, %.o, $(PREP_SOURCES))

# Neural network sources (exclude train.c which has stb_image conflicts)
NN_SOURCES = ../neural_network/src/neural_network.c ../neural_network/src/activation_functions.c \
             ../neural_network/src/nodes_loss.c \
             ../neural_network/src/learn.c ../neural_network/src/save_load.c ../neural_network/src/utils.c
NN_OBJECTS = $(patsubst %.c, %.o, $(NN_SOURCES))

SOLVER_SOURCES = ../solver.c
SOLVER_OBJECTS = $(patsubst %.c, %.o, $(SOLVER_SOURCES))


ui: ui.c $(PREP_OBJECTS) $(NN_OBJECTS) $(SOLVER_OBJECTS)
	$(CC) $(CFLAGS) $(GTK_CFLAGS) $(INCLUDES) -o $@ $^ $(MATH_LIB) $(GTK_LIBS)

# Règle pour compiler les fichiers C du preprocesseur en fichiers objets (.o)
../preporcessor/src/%.o: ../preporcessor/src/%.c
	$(CC) $(CFLAGS) $(GTK_CFLAGS) $(INCLUDES) -c $< -o $@

# Règle pour compiler les fichiers du réseau de neurones
../neural_network/src/%.o: ../neural_network/src/%.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

# Règle pour compiler le solver
../%.o: ../%.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

.PHONY: clean

clean:
	rm -f ui
	rm -f $(PREP_OBJECTS)
	rm -f $(NN_OBJECTS)
	rm -f $(SOLVER_OBJECTS)
