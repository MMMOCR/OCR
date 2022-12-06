CC = gcc
CFLAGS = -Wall -Wextra -g `pkg-config --cflags sdl2`
LDLIBS = `pkg-config --libs sdl2` -lSDL2_image -lm
SRC = empty_cell.c
OBJ = ${SRC:.c=.o}
EXE = main

all: $(EXE)

$(EXE): $(OBJ)
	$(CC) -o $@ $^ $(LDLIBS)

%.o: %.c
	$(CC) -c $< -o $@ $(CFLAGS)

clean:
	rm $(EXE) $(OBJ)