# Makefile

CC := gcc

PACKAGES := gtk+-3.0 sdl2 SDL2_image SDL2_ttf gdk-3.0 gdk-x11-3.0
GUIPACKAGES := gtk+-3.0 sdl SDL_image SDL_ttf SDL_gfx

ifneq ($(OS),Windows_NT)
	UNAME_S := $(shell uname -s)
	ifeq ($(UNAME_S),Darwin)
		LIBS := --libs-only-l
    endif
endif

OCR ?= ocr-

LIBS ?= --libs

CFLAGS := -Wall -Wextra $(shell pkg-config $(PACKAGES) --cflags) -g3 #-fsanitize=address
GUICFLAGS := -Wall -Wextra $(shell pkg-config $(GUIPACKAGES) --cflags) -g3 #-fsanitize=address
CPPFLAGS := 
LDLIBS := $(shell pkg-config $(PACKAGES) $(LIBS)) -lm
GUILDLIBS := $(shell pkg-config $(GUIPACKAGES) $(LIBS)) -lm
LDFLAGS :=

ifeq ($(DEBUG), 1)
	CFLAGS += -g3
	CFLAGS += -fsanitize=address
	LDFLAGS += -fsanitize=address
endif

OUT := neuralnetwork solver linesdetection imageutils gui

NN_SRCS = neuralnetwork/functions.c neuralnetwork/job.c utils/empty_cell.c neuralnetwork/loadset.c neuralnetwork/NN.c neuralnetwork/tools.c neuralnetwork/train.c 

LINES_SRCS = utils/linesdetection.c utils/rotateutils.c utils/resize.c

WRITE_SRCS = utils/writeonim.c

UTILS_SRCS = utils/gaussian_blur.c utils/resize.c utils/sobel.c utils/erosion_dilation.c utils/imageutils.c utils/otsu.c

SOLVER_SRCS = solver/solver.c

GUI_SRCS = gui/main.c gui/instructions.c gui/impl/SDLRenderer/sypbc_impl.h gui/impl/SDLRenderer/sypbc_sdlrenderer.h gui/utils/input.c gui/utils/colors.c gui/utils/osapi.c gui/utils/animations/linear.c gui/widgets/button.c

all: $(OUT)

neuralnetwork: FORCE
	$(CC) $(NN_SRCS) $(CFLAGS) -o bin/$@ $(LDLIBS)

solver: FORCE
	$(CC) $(SOLVER_SRCS) $(CFLAGS) -o bin/$@ $(LDLIBS)

linesdetection: FORCE
	$(CC) $(LINES_SRCS) $(CFLAGS) -o bin/$@ $(LDLIBS)

imageutils: FORCE
	$(CC) $(UTILS_SRCS) $(CFLAGS) -o bin/$@ $(LDLIBS)

writeonim: FORCE
	$(CC) $(WRITE_SRCS) $(CFLAGS) -o bin/$@ $(LDLIBS)

gui: FORCE
	$(CC) $(GUI_SRCS) $(GUICFLAGS) -o bin/$@ $(GUILDLIBS)

FORCE:

clean:
	rm -f $(OUT:%=bin/%)

.PHONY: all clean
