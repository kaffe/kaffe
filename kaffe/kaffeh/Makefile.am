# Makefile.in for kaffeh - kaffe native code stub generator.
#
# Copyright (c) 1996, 1997, 1998, 1999
#	Transvirtual Technologies, Inc.  All rights reserved.
#
# See the file "license.terms" for information on usage and redistribution 
# of this file. 

AM_CPPFLAGS = \
	-I../kaffevm \
	-I$(srcdir)/../kaffevm \
	-DKAFFEH \
	-I$(top_builddir)/include \
	-I$(top_builddir)/config \
	-I$(top_srcdir)/kaffe/kaffevm/jni

bin_PROGRAMS = kaffeh

GEN_SRCS = \
	constants.c \
	debug.c \
	file.c \
	inflate.c \
	jar.c \
	readClass.c \
	utf8const.c

DISTCLEANFILES = $(GEN_SRCS)

kaffeh_CFLAGS = \
	$(CFLAGS_PG)

kaffeh_SOURCES = \
	main.c \
	mem.c \
	sigs.c \
	support.c

nodist_kaffeh_SOURCES = $(GEN_SRCS)

noinst_HEADERS = \
	kaffeh-support.h \
	java_lang_ClassLoader.h \
	java_lang_Object.h \
	java_lang_String.h

$(GEN_SRCS):
	$(SHELL) $(REGEN_FORWARD) $(srcdir)/../kaffevm/$@ $@

##dist-hook:
##	@for file in $(DISTCLEANFILES); do \
##	  rm -f $(distdir)/$$file; \
##	done
