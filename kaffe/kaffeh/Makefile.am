# Makefile.in for kaffeh - kaffe native code stub generator.
#
# Copyright (c) 1996, 1997, 1998, 1999
#	Transvirtual Technologies, Inc.  All rights reserved.
#
# See the file "license.terms" for information on usage and redistribution 
# of this file. 

AM_CPPFLAGS = -I../kaffevm -I$(srcdir)/../kaffevm -DKAFFEH -I$(top_builddir)/include

bin_PROGRAMS = kaffeh

GEN_SRCS = inflate.c jar.c utf8const.c readClass.c constants.c debug.c

DISTCLEANFILES = $(GEN_SRCS)

kaffeh_SOURCES = sigs.c support.c main.c mem.c

nodist_kaffeh_SOURCES = $(GEN_SRCS)

noinst_HEADERS = kaffeh-support.h \
	java_lang_ClassLoader.h \
	java_lang_Object.h \
	java_lang_String.h

$(GEN_SRCS):
	$(SHELL) $(REGEN_FORWARD) $(srcdir)/../kaffevm/$@ $@

##dist-hook:
##	@for file in $(DISTCLEANFILES); do \
##	  rm -f $(distdir)/$$file; \
##	done
