# Makefile

CC ?= gcc

PACKAGES := gtk+-3.0 sdl2 SDL2_image gdk-3.0 gdk-x11-3.0

ifneq ($(OS),Windows_NT)
	UNAME_S := $(shell uname -s)
	ifeq ($(UNAME_S),Darwin)
		LIBS := --libs-only-l
    endif
endif

LIBS ?= --libs

CFLAGS := -Wall -Wextra $(shell pkg-config $(PACKAGES) --cflags) -g3 #-fsanitize=address
CPPFLAGS := -MD
LDLIBS := $(shell pkg-config $(PACKAGES) $(LIBS)) -lm
LDFLAGS :=

ifeq ($(DEBUG), 1)
	CFLAGS += -g3
endif

IMG ?= 5
EXT ?= jpeg

OUT := utils/linesdetection utils/imageutils gui/interface_rotate
OBJS := utils/rotateutils.o

DEPS := $(OBJS:%.o=%.d)
DEPS += $(OUT:%=%.d)

utils/linesdetection: utils/rotateutils.o
gui/interface_rotate: utils/rotateutils.o

$(DEPS):
	$(CC) $(CFLAGS) $(CPPFLAGS) $(@:%.d=%.c) $(LDFLAGS) $(LDLIBS)

$(OUT): %.d
	$(CC) $(CFLAGS) $(CPPFLAGS) $(^:%.h=) $(LDLIBS) $(LDFLAGS) -o $@

$(OBJ):
	$(CC) $(CFLAGS) $(CPPFLAGS) -c $(@:%.o=%.c) -o $@

test_linedetection: utils/linesdetection
	./$< ./images/ocr-$(IMG).$(EXT)

test_gui: gui/interface_rotate
	./$< ./images/ocr-1.png

test_imageutils: utils/imageutils
	./$< ./images/sudoku3.jpeg
	
test_solver: solver/solver
	./$< ./solver/samples/sample1

clean:
	rm -rf $(OUT)
	rm -rf $(OUT:%=%.o)
	rm -rf $(DEPS)

.PHONY: clean test_gui test_linedetection test_imageutils

-include $(DEPS)
