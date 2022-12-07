# Makefile

CC = gcc
CPPFLAGS = 
CFLAGS = -g `pkg-config --cflags sdl2`
LDFLAGS =
LDLIBS = `pkg-config --libs sdl2` -lm


SRC = NN.c train.c tools.c functions.c loadset.c
OBJ = ${SRC:.c=.o}
EXE = ${SRC:.c=}

all: main

main:
	gcc $(CFLAGS) -o NN *.c $(LDLIBS)

clean:
	${RM} ${OBJ}
	${RM} ${EXE}

# END
