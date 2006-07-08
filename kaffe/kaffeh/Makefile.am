# Makefile.in for kaffeh - kaffe native code stub generator.
#
# Copyright (c) 1996, 1997, 1998, 1999
#	Transvirtual Technologies, Inc.  All rights reserved.
#
# See the file "license.terms" for information on usage and redistribution 
# of this file. 

AM_CFLAGS= $(KAFFE_CFLAGS)

if COND_SYSTEM_ZLIB
LINKED_ZIP_LIBS=$(ZIP_LIBS)
else
LINKED_ZIP_LIBS=../../external/gcc/zlib/libz.la
MAYBE_INTERNAL_ZIP_INCLUDES=-I$(srcdir)/../../external/gcc/zlib/
endif

AM_CPPFLAGS = \
	-I../kaffevm \
	-I$(srcdir)/../kaffevm \
	-DKAFFEH \
	-I$(top_builddir)/include \
	-I$(top_builddir)/config \
	-I$(top_srcdir)/kaffe/kaffevm/jni \
	$(MAYBE_INTERNAL_ZIP_INCLUDES)

bin_PROGRAMS = kaffeh

GEN_SRCS = \
	constants.c \
	debug.c \
	file.c \
	jar.c \
	readClass.c \
	utf8const.c \
	binreloc.c

DISTCLEANFILES = $(GEN_SRCS)

kaffeh_CFLAGS = \
	$(CFLAGS_PG)

kaffeh_LDADD = \
	$(LTLIBINTL) \
	$(LINKED_ZIP_LIBS)

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
