# Makefile.in for kaffeh - kaffe native code stub generator.
#
# Copyright (c) 1996, 1997, 1998, 1999
#	Transvirtual Technologies, Inc.  All rights reserved.
#
# See the file "license.terms" for information on usage and redistribution 
# of this file. 

INCLUDES = -I../kaffevm -I$(srcdir)/../kaffevm -DKAFFEH

bin_PROGRAMS = kaffeh

GEN_SRCS = inflate.c jar.c utf8const.c readClass.c constants.c debug.c

DISTCLEANFILES = $(GEN_SRCS)

kaffeh_SOURCES = $(GEN_SRCS) sigs.c support.c main.c mem.c

noinst_HEADERS = kaffeh-support.h \
	java_lang_ClassLoader.h \
	java_lang_Object.h

$(GEN_SRCS):
	$(SHELL) $(REGEN_FORWARD) $(srcdir)/../kaffevm/$@ $@

dist-hook:
	@for file in $(DISTCLEANFILES); do \
	  rm -f $(distdir)/$$file; \
	done
