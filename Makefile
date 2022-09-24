# Makefile

CC = gcc

CFLAGS = -Wall -Wextra -pedantic $(shell pkg-config sdl2 SDL2_image --cflags --libs)

imageutils: utils/imageutils

test: utils/imageutils
	./utils/main /home/malossa/2022-09-22-155433_1920x1080_scrot.png

.PHONY: test

clean:
	rm -rf utils/imageutils
