CC = gcc
CFLAGS = -g
LDLIBS = -lm
SRC = NN.c train.c tools.c functions.c loadset.c
OBJ = ${SRC:.c=.o}
EXE = NN

all: $(EXE)

$(EXE): $(OBJ)
	$(CC) -o $@ $^ $(LDLIBS)

%.o: %.c
	$(CC) -c $< -o $@ $(CFLAGS)

clean:
	rm $(EXE) $(OBJ)