CC = gcc
CFLAGS = -Wall -Wextra -g -fsanitize=address
INCLUDES = -I./stb
INCLUDES = -I./a/stb
LDFLAGS = -lm
EXE = main


prep:
	$(CC) $(SOURCE_PREP) $(CFLAGS) $(INCLUDES) $(LDFLAGS) -o $(EXE)
	./$(EXE) $(IMG)
	rm $(EXE)

neur:
	$(CC) $(SOURCE_NEUR) $(CFLAGS) $(INCLUDES) $(LDFLAGS) -o $(EXE)
	./$(EXE)
	rm $(EXE)


IMG = ./imgs/level_1_image_1.png

SOURCE_PREP = ./preporcessor/src/*.c
SOURCE_NEUR = ./neural_network/src/*.c
