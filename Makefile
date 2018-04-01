CFLAGS+=-Wall -Wextra -O2 -D_POSIX_C_SOURCE=200112L
LDFLAGS+=-lncurses

PACKAGE=pinkycurses
PROG=ncurses.c

all:
	$(CC) $(CFLAGS) $(LDFLAGS) -o $(PACKAGE) $(PROG)

install: 
	install -D -s -m 755 $(PACKAGE) /usr/bin/$(PACKAGE)

clean:
	rm -f /usr/bin/$(PACKAGE)

.PHONY: all install clean pinkycurses
