# Makefile

CC := gcc

PACKAGES := gtk+-3.0 sdl2 SDL2_image gdk-3.0 gdk-x11-3.0

DEPS := $($(shell find . *.c):%.c=%.h)

CFLAGS := -Wall -Wextra $(shell pkg-config $(PACKAGES) --cflags)
CPPFLAGS := -MMD
LDLIBS := $(shell pkg-config $(PACKAGES) --libs) -lm 
LDFLAGS :=

ifeq ($(DEBUG), 1)
	CFLAGS += -g3
endif

IMG ?= 2

utils/linesdetection: utils/linesdetection.c utils/linesdetection.h
	$(CC) -o utils/linesdetection utils/linesdetection.c $(CFLAGS) $(CPPFLAGS) $(LDLIBS) $(LDFLAGS)

test: utils/linesdetection
#	./utils/linesdetection ~/Documents/1615279962287.png
	./utils/linesdetection ./images/ocr-$(IMG).png

clean:
	rm -rf utils/linesdetection
	rm -rf $(DEPS)

.PHONY: test clean

-include $(DEPS)
