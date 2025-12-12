CC = gcc
CFLAGS = -Wall -Wextra -g -fsanitize=address
INCLUDES = -I./stb
LDFLAGS = -lm
EXE = main
UI_EXE = ui/ui

.PHONY: prep neur
#prep:
#	$(CC) $(SOURCE_PREP) $(CFLAGS) $(INCLUDES) $(LDFLAGS) -o $(EXE)
#	./$(EXE) $(IMG)
#	rm $(EXE)

all: ui
neur:
	$(CC) $(SOURCE_NEUR) $(CFLAGS) $(INCLUDES) $(LDFLAGS) -o $(EXE)
	./$(EXE)
	rm $(EXE)

.PHONY: ui solver
ui:
	$(MAKE) -C ./ui
	./${UI_EXE}


.PHONY: clean
clean:
	rm -f $(EXE)
	rm -f ${UI_EXE}
	rm -f solver
	rm -f ./preporcessor/src/*.o
	rm -f ./neural_network/src/*.o
	rm -f *.o
	$(MAKE) -C ./ui clean
	rm -r dump*/
	rm output.png
	rm solved_output.png

IMG = ./imgs/level_1_image_2.png

SOURCE_PREP = ./preporcessor/src/*.c
SOURCE_NEUR = ./neural_network/src/*.c
