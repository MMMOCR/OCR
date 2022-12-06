# Makefile

CC = gcc
CPPFLAGS = 
CFLAGS = -Wall -Wextra -O3 `pkg-config --cflags sdl2`
LDFLAGS =
LDLIBS = `pkg-config --libs sdl2` -lm


SRC = NN.c train.c tools.c job.c functions.c
OBJ = ${SRC:.c=.o}
EXE = ${SRC:.c=}
main:

main:
	gcc -lm -Wall -Wextra -Werror -O1 -o NN *.c

clean:
	${RM} ${OBJ}
	${RM} ${EXE}

# END
