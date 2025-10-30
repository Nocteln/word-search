CC = gcc
CFLAGS = -Wall -Wextra -g 
INCLUDES = -I./stb -I./include 
LDFLAGS = -lm

O_FILES_PATH = ./objs
EXE = main


prep:
	make neur_lib
	$(CC) $(O_FILES_PATH)/* $(SOURCE_PREP) $(CFLAGS) $(INCLUDES) $(LDFLAGS) -o $(EXE)
	./$(EXE) $(IMG)
	rm $(EXE)

neur:
	$(CC) $(SOURCE_NEUR) $(CFLAGS) $(INCLUDES) $(LDFLAGS) -o $(EXE)
	./$(EXE)
	rm $(EXE)


neur_lib:
	# $(CC) -c $(SOURCE_NEUR) $(CFLAGS) $(INCLUDES) $(LDFLAGS) -o $(O_FILES_PATH)
	# TODO: dirty but works for now
	find $(SOURCE_NEUR) -name "*.c" -print0 | xargs -0 -I {} sh -c \
	'gcc -c "{}" $(CFLAGS) $(INCLUDES) $(LDFLAGS) -o "$(O_FILES_PATH)/$$(basename {} .c).o"'

clean:
	rm -f $(O_FILES_PATH)/*.o $(EXE)




IMG = ./imgs/level_1_image_1.png

SOURCE_PREP = ./preporcessor/src/*.c
SOURCE_NEUR = ./neural_network/src/*.c
