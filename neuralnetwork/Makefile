CC = gcc
CFLAGS = -g  `pkg-config --cflags SDL2_image sdl2`
LDLIBS = -lm `pkg-config --libs SDL2_image sdl2`
SRC = NN.c train.c tools.c functions.c loadset.c job.c
OBJ = ${SRC:.c=.o}
EXE = NN

all: $(EXE)

$(EXE): $(OBJ)
	$(CC) -o $@ $^ $(LDLIBS)

%.o: %.c
	$(CC) -c $< -o $@ $(CFLAGS)

clean:
	rm $(EXE) $(OBJ)