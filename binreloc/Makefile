CC=gcc
AR=ar
RANLIB=randlib
BINRELOC=-DENABLE_BINRELOC
CFLAGS=-Wall -g $(BINRELOC) -DBR_PTHREADS=0

PACKAGE=binreloc
VERSION=1.3

.PHONY: clean test dist distdir

test: test.c prefix.c prefix.h libtest.so
	$(CC) $(CFLAGS) test.c prefix.c -o test -ldl -L. -ltest

libtest.so: libtest.c prefix.c prefix.h
	$(CC) $(CFLAGS) libtest.c prefix.c -o libtest.so -fPIC -shared -ldl

clean:
	rm -f test libtest.so

distdir:
	rm -rf $(PACKAGE)-$(VERSION)
	mkdir $(PACKAGE)-$(VERSION)
	cp README ChangeLog Makefile prefix.c prefix.h libtest.c test.c binreloc.m4 $(PACKAGE)-$(VERSION)/

dist: distdir
	rm -f $(PACKAGE)-$(VERSION).tar.gz
	tar -cf $(PACKAGE)-$(VERSION).tar $(PACKAGE)-$(VERSION)
	gzip --best $(PACKAGE)-$(VERSION).tar
	rm -rf $(PACKAGE)-$(VERSION)
