# Makefile.frag for kaffevm l4threads
#

AM_CFLAGS= $(KAFFE_CFLAGS)

noinst_LTLIBRARIES = libkthread.la
libkthread_la_LDFLAGS = $(KVMLIBFLAGS)
libkthread_la_SOURCES = l4thread.c signal.c syscalls.c
EXTRA_DIST = Makefile.frag jsignal.h jthread.h lock-impl.h thread-impl.h

@engine_frag@

AM_CPPFLAGS = \
	-I$(srcdir) \
	-I../.. \
	-I$(srcdir)/../.. \
	-I$(top_builddir)/config \
	-I$(top_srcdir)/config \
	-I$(top_builddir)/include \
	-I$(top_srcdir)/include \
	-I$(top_builddir)/include \
        -I$(top_srcdir)/config/$(Khost_cpu) \
        -I$(top_srcdir)/config/$(Khost_cpu)/$(Khost_os) 

DEFS = $(ENGINE_DEFS) -DKVER=\"$(KVER)\"
