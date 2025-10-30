CC = gcc
CFLAGS = -Wall -Wextra -g -fsanitize=address
GTK_CFLAGS = $(shell pkg-config --cflags gtk+-3.0)
GTK_LIBS = $(shell pkg-config --libs gtk+-3.0)
MATH_LIB = -lm

INCLUDES = -I. -I../preporcessor/src -I../neural_network/src -I../stb -I..

PREP_SOURCES = ../preporcessor/src/filters.c ../preporcessor/src/main.c \
               ../preporcessor/src/utils.c ../preporcessor/src/process.c
NN_SOURCES = ../neural_network/src/neural_network.c \
             ../neural_network/src/activation_functions.c \
             ../neural_network/src/nodes_loss.c \
             ../neural_network/src/learn.c \
             ../neural_network/src/save_load.c \
             ../neural_network/src/utils.c

OBJECTS = $(PREP_SOURCES:.c=.o) $(NN_SOURCES:.c=.o)

ui: ui.c $(OBJECTS)
	$(CC) $(CFLAGS) $(GTK_CFLAGS) $(INCLUDES) -o $@ $^ $(MATH_LIB) $(GTK_LIBS)

../preporcessor/src/%.o: ../preporcessor/src/%.c
	$(CC) $(CFLAGS) $(GTK_CFLAGS) $(INCLUDES) -c $< -o $@

../neural_network/src/%.o: ../neural_network/src/%.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

../%.o: ../%.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

.PHONY: clean
clean:
	rm -f ui $(OBJECTS)