# Makefile

CC = gcc

PACKAGES= gtk+-3.0 sdl2 SDL2_image gdk-3.0 gdk-x11-3.0

CFLAGS = -Wall -Wextra $(shell pkg-config $(PACKAGES) --cflags) -fsanitize=address -g
CPPFLAGS = 
LDLIBS = $(shell pkg-config $(PACKAGES) --libs) -lm 
LDFLAGS = -fsanitize=address

utils/rotateutils.o: utils/rotateutils.c utils/rotateutils.h

gui/interface_rotate: gui/interface_rotate.c gui/interface_rotate.h utils/rotateutils.o 

test: gui/interface_rotate
	./gui/interface_rotate ~/Documents/1615279962287.png

clean:
	rm -rf gui/interface_rotate
	rm -rf utils/imageutils.o

.PHONY: test clean
