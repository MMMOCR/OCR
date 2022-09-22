# Makefile

CC = gcc

CFLAGS = -Wall -Wextra -pedantic $(shell pkg-config gtk+-3.0 sdl2 --cflags)
LDLIBS = $(shell pkg-config gtk+-3.0 sdl2 --libs)

interface_rotate: gui/interface_rotate

test: interface_rotate
	./gui/interface_rotate aaa

.PHONY: test
