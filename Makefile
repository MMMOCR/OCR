# Makefile

CC := gcc

PACKAGES := gtk+-3.0 sdl2 SDL2_image gdk-3.0 gdk-x11-3.0

DEPS := $($(shell find . *.c):%.c=%.h)

CFLAGS := -Wall -Wextra $(shell pkg-config $(PACKAGES) --cflags)
CPPFLAGS := -MMD
LDLIBS := $(shell pkg-config $(PACKAGES) --libs) -lm 
LDFLAGS :=

IMG ?= 2

ifeq ($(DEBUG), 1)
	CFLAGS += -g3
endif


utils/rotateutils.o: utils/rotateutils.c utils/rotateutils.h
	$(CC) -o utils/rotateutils.o -c utils/rotateutils.c $(CFLAGS) $(CPPFLAGS) $(LDLIBS) $(LDFLAGS)

gui/interface_rotate: gui/interface_rotate.c gui/interface_rotate.h utils/rotateutils.o 
	$(CC) -o gui/interface_rotate gui/interface_rotate.c utils/rotateutils.o $(CFLAGS) $(CPPFLAGS) $(LDLIBS) $(LDFLAGS)

utils/linesdetection: utils/linesdetection.c utils/linesdetection.h
	$(CC) -o utils/linesdetection utils/linesdetection.c $(CFLAGS) $(CPPFLAGS) $(LDLIBS) $(LDFLAGS)

test: utils/linesdetection
#	./utils/linesdetection ~/Documents/1615279962287.png
	./utils/linesdetection ./images/ocr-$(IMG).png

clean:
	rm -rf utils/linesdetection
  rm -rf gui/interface_rotate
	rm -rf utils/imageutils.o
	rm -rf $(DEPS)

.PHONY: test clean

-include $(DEPS)
