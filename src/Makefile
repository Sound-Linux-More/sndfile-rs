PROGNAME      = sndfile-rs sndfile-denoise
CC            = gcc
CFLAGS        = -DUNIX -O2 -Wall
LDFLAGS       = -s
LIBS          = -lsndfile
VER           = 0
VERB          = 20171122
COMMON        = 
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

sndfile-rs: sndrs.c $(COMMON)
	$(CC) $(CFLAGS) $(LIBS) $^ $(LDFLAGS) -o $@

sndfile-denoise: snddenoise.c $(COMMON)
	$(CC) $(CFLAGS) $(LIBS) $^ $(LDFLAGS) -o $@

install: $(PROGNAME)
	$(INSTALL) -d $(PREFIX)/bin
	$(INSTALL) -m 0755 sndfile-rs $(PREFIX)/bin/
	$(INSTALL) -m 0755 sndfile-denoise $(PREFIX)/bin/
