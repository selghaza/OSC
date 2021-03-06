include config.mk

CC = cc
CFLAGS = -std=c99 -Wall -Wextra -pedantic -Wno-unused-parameter -Os ${JACK_CFLAGS}
LDLIBS = ${JACK_LDLIBS}

BINS = \
	sin \
	sdl

all: ${BINS}

LDLIBS.sin = -lm

LDLIBS.tbl = ${SF_LDLIBS}
CFLAGS.tbl = ${SF_CFLAGS}
CFLAGS.sdl = ${SDL_CFLAGS}
LDLIBS.sdl = ${SDL_LDLIBS}

.c:
	${CC} ${CFLAGS} ${CFLAGS.$@} -o $@ $< ${LDLIBS} ${LDLIBS.$@}

clean:
	rm -f ${BINS} *.o *.exe a.out

.PHONY: all clean
