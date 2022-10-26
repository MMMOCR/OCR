# Makefile

CC ?= gcc

PACKAGES := gtk+-3.0 sdl2 SDL2_image gdk-3.0 gdk-x11-3.0

DEPS := $($(shell find . *.c):%.c=%.h)

CFLAGS := -Wall -Wextra $(shell pkg-config $(PACKAGES) --cflags) -g3 #-fsanitize=address
CPPFLAGS := -MMD
LDLIBS := $(shell pkg-config $(PACKAGES) --libs-only-l) -lm
LDFLAGS :=

IMG ?= 2

ifeq ($(DEBUG), 1)
	CFLAGS += -g3
endif

IMG ?= 2
EXT ?= png

utils/rotateutils.o: utils/rotateutils.c utils/rotateutils.h
	$(CC) -o utils/rotateutils.o -c utils/rotateutils.c $(CFLAGS) $(CPPFLAGS) $(LDLIBS) $(LDFLAGS)

gui/interface_rotate: gui/interface_rotate.c gui/interface_rotate.h utils/rotateutils.o 
	$(CC) -o gui/interface_rotate gui/interface_rotate.c utils/rotateutils.o $(CFLAGS) $(CPPFLAGS) $(LDLIBS) $(LDFLAGS)

utils/linesdetection: utils/linesdetection.c utils/linesdetection.h utils/rotateutils.o
	$(CC) -o utils/linesdetection utils/linesdetection.c utils/rotateutils.o $(CFLAGS) $(CPPFLAGS) $(LDLIBS) $(LDFLAGS)

test: utils/linesdetection
	./utils/linesdetection ./images/ocr-$(IMG).$(EXT)
#	./utils/linesdetection ~/Documents/1615279962287.png

test_gui: gui/interface_rotate
	./gui/interface_rotate ./images/ocr-1.png

clean:
	rm -rf utils/linesdetection
	rm -rf gui/interface_rotate
	rm -rf utils/imageutils.o
	rm -rf $(DEPS)

.PHONY: test clean

-include $(DEPS)
