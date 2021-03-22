PROGNAME      = sndfile-rs sndfile-denoise sndfile-las
CC            = gcc
CFLAGS        = -DUNIX -O2 -Wall
LDFLAGS       = -s
LIBS          = -lsndfile -lm
VER           = 0
VERB          = 20210322
COMMON        = 
SRCS          = src
PREFIX        = /usr/local
INCPREFIX     = $(PREFIX)/include
LIBPREFIX     = $(PREFIX)/lib
MANPREFIX     = $(PREFIX)/share/man/man1
INSTALL       = install
LN            = ln -fs

.PHONY: all clean install

all: $(PROGNAME)

clean:
	rm -f $(PROGNAME)

sndfile-rs: $(SRCS)/sndrs.c $(COMMON)
	$(CC) $(CFLAGS) $(LIBS) $^ $(LDFLAGS) -o $@

sndfile-denoise: $(SRCS)/snddenoise.c $(COMMON)
	$(CC) $(CFLAGS) $(LIBS) $^ $(LDFLAGS) -o $@

sndfile-las: $(SRCS)/zxfft/zx_fft.c $(SRCS)/zxfft/zx_math.c $(SRCS)/sndlas.c $(COMMON)
	$(CC) $(CFLAGS) $(LIBS) $^ $(LDFLAGS) -o $@

install: $(PROGNAME)
	$(INSTALL) -d $(PREFIX)/bin
	$(INSTALL) -m 0755 $(PROGNAME) $(PREFIX)/bin/
