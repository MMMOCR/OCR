# Makefile

CC = gcc

PACKAGES= gtk+-3.0 sdl2 SDL2_Image gdk-3.0 gdk-x11-3.0

CFLAGS = -Wall -Wextra -pedantic $(shell pkg-config $(PACKAGES) --cflags)
LDLIBS = $(shell pkg-config $(PACKAGES) --libs) -lm

utils/rotateutils.o: utils/rotateutils.c

gui/interface_rotate: gui/interface_rotate.c utils/rotateutils.o

test: gui/interface_rotate
	./gui/interface_rotate ~/Documents/1615279962287.png

clean:
	rm -rf gui/interface_rotate
	rm -rf utils/imageutils.o

.PHONY: test clean
