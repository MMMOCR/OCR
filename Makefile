# Makefile

CC := gcc

PACKAGES := gtk+-3.0 sdl2 SDL2_image gdk-3.0 gdk-x11-3.0

ifneq ($(OS),Windows_NT)
	UNAME_S := $(shell uname -s)
	ifeq ($(UNAME_S),Darwin)
		LIBS := --libs-only-l
    endif
endif

LIBS ?= --libs

CFLAGS := -Wall -Wextra $(shell pkg-config $(PACKAGES) --cflags) -g3 #-fsanitize=address
CPPFLAGS := 
LDLIBS := $(shell pkg-config $(PACKAGES) $(LIBS)) -lm
LDFLAGS :=

ifeq ($(DEBUG), 1)
	CFLAGS += -g3
	CFLAGS += -fsanitize=address
	LDFLAGS += -fsanitize=address
endif

IMG ?= 5
EXT ?= jpeg

OUT := linesdetection imageutils interface_rotate solver NN
OBJS := utils/rotateutils.o neuralnetwork/functions.o neuralnetwork/job.o neuralnetwork/save.o neuralnetwork/tools.o neuralnetwork/train.o utils/otsu.o utils/gaussian_blur.o utils/sobel.o utils/imageutils.o


DEPS := $(OUT:%=%.d)
DEPS += $(OBJS:%.o=%.d)

all: $(OUT)

solver: solver/solver.o
gui/interface_rotate linesdetection: utils/rotateutils.o utils/linesdetection.o
imageutils: utils/otsu.o utils/gaussian_blur.o utils/sobel.o utils/imageutils.o utils/erosion_dilation.o utils/resize.o
NN: neuralnetwork/functions.o neuralnetwork/job.o neuralnetwork/save.o neuralnetwork/tools.o neuralnetwork/train.o neuralnetwork/NN.o

$(OUT):
	$(CC) $(CFLAGS) $(CPPFLAGS) $^ $(LDLIBS) $(LDFLAGS) -o bin/$@

$(OBJ):
	$(CC) $(CFLAGS) $(CPPFLAGS) -c $(@:%.o=%.c) -o $@

test_linedetection: linesdetection
	./bin/linesdetection /images/ocr-$(IMG).$(EXT)

test_gui: gui/interface_rotate
	./$< ./images/ocr-1.png

test_imageutils: imageutils
	./bin/imageutils ./images/sudoku2.jpeg
	
test_solver: solver
	./bin/solver ./solver/samples/sample1

test_NN: NN
	# ./bin/NN 

clean:
	rm -rf $(OUT)
	rm -rf $(OBJS)
	rm -rf $(OUT:%=%.o)
	rm -rf $(DEPS)
	rm -rf $(DEPS:%.d=%.dSYM)

.PHONY: clean test_gui test_linedetection test_imageutils test_solver test_NN all
