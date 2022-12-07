# Makefile

CC = gcc
CPPFLAGS = 
CFLAGS = -g3 `pkg-config --cflags sdl2`
LDFLAGS =
LDLIBS = `pkg-config --libs sdl2` -lm -fsanitize=address


SRC = NN.c train.c tools.c functions.c loadset.c
OBJ = ${SRC:.c=.o}
EXE = ${SRC:.c=}

all: NN

NN:
	gcc $(CFLAGS) -o NN *.c $(LDLIBS)

clean:
	${RM} ${OBJ}
	${RM} ${EXE}

# END
