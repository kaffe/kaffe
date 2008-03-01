# Makefile.frag for kaffevm l4threads
#

noinst_LTLIBRARIES = libkthread.la
libkthread_la_LDFLAGS = $(KVMLIBFLAGS)
libkthread_la_SOURCES = \
	l4thread.c \
	l4thread.h \
	signal.c \
	syscalls.c \
	jsignal.h \
	jthread.h \
	lock-impl.h \
	thread-impl.h

EXTRA_DIST = Makefile.frag 

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
