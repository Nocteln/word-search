CC = gcc
CFLAGS = -Wall -Wextra -g -fsanitize=address
INCLUDES = -I./stb
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

ui:
	$(MAKE) -C ./ui

clean:
	rm -f $(EXE)
	rm -f ./preporcessor/src/*.o
	rm -f ./neural_network/src/*.o
	rm -f *.o
	$(MAKE) -C ./ui clean

IMG = ./imgs/level_1_image_2.png

SOURCE_PREP = ./preporcessor/src/*.c
SOURCE_NEUR = ./neural_network/src/*.c
