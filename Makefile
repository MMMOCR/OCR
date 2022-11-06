# Makefile

CC ?= gcc

PACKAGES := gtk+-3.0 sdl2 SDL2_image gdk-3.0 gdk-x11-3.0

DEPS := $($(shell find . *.c):%.c=%.h)

ifneq ($(OS),Windows_NT)
	UNAME_S := $(shell uname -s)
	ifeq ($(UNAME_S),Darwin)
		LIBS := --libs-only-l
    endif
endif

LIBS ?= --libs

CFLAGS := -Wall -Wextra $(shell pkg-config $(PACKAGES) --cflags) -g3 #-fsanitize=address
CPPFLAGS := -MMD
LDLIBS := $(shell pkg-config $(PACKAGES) $(LIBS)) -lm
LDFLAGS :=

ifeq ($(DEBUG), 1)
	CFLAGS += -g3
endif

IMG ?= 2
EXT ?= png

OUT := utils/linesdetection utils/imageutils gui/interface_rotate

$(OUT): $(OUT:%:%.c) utils/rotateutils.o
	$(CC) $(CFLAGS) $(CPPFLAGS) $@.c $^ $(LDLIBS) $(LDFLAGS) -o $@

utils/rotateutils.o: utils/rotateutils.c utils/rotateutils.h
	$(CC) $(CFLAGS) $(CPPFLAGS) -c $< $(LDLIBS) $(LDFLAGS) -o $@

gui/interface_rotate: gui/interface_rotate.c gui/interface_rotate.h utils/rotateutils.o 
	$(CC) $(CFLAGS) $(CPPFLAGS) $^ $(LDLIBS) $(LDFLAGS) -o $@

utils/imageutils: utils/imageutils.c
	$(CC) $(CFLAGS) $(CPPFLAGS) $^ $(LDLIBS) $(LDFLAGS) -o $@

test_linedetection: utils/linesdetection
	./$< ./images/ocr-5.jpeg

test_gui: gui/interface_rotate
	./$< ./images/ocr-1.png

test_imageutils: utils/imageutils
	./$< /home/malossa/2022-09-22-155433_1920x1080_scrot.png

clean:
	rm -rf $(OUT)
	rm -rf $(OUT:%:%.o)
	rm -rf $(DEPS)

.PHONY: clean test_gui test_linedetection test_imageutils

-include $(DEPS)
