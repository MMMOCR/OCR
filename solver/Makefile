CC = gcc
CFLAGS = -Wall -Wextra -g
LDLIBS = -lm
SRC = hexadoku.c
OBJ = ${SRC:.c=.o}
EXE = main

all: $(EXE)

$(EXE): $(OBJ)
	$(CC) -o $@ $^ $(LDLIBS)

%.o: %.c
	$(CC) -c $< -o $@ $(CFLAGS)

clean:
	rm $(EXE) $(OBJ)