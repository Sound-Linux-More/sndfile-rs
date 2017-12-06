PROGNAME      = sndfile-rs
CC            = gcc
CFLAGS        = -DUNIX -O2 -Wall
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
	$(CC) $(CFLAGS) $(LIBS) $^ -s -o $@

install: $(PROGNAME)
	$(INSTALL) -d $(PREFIX)/bin
	$(INSTALL) -m 0755 $(PROGNAME) $(PREFIX)/bin/
