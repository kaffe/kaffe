# Makefile.in for kaffeh - kaffe native code stub generator.
#
# Copyright (c) 1996, 1997, 1998, 1999
#	Transvirtual Technologies, Inc.  All rights reserved.
#
# See the file "license.terms" for information on usage and redistribution 
# of this file. 

INCLUDES = -I../kaffevm -I$(srcdir)/../kaffevm -DKAFFEH

bin_PROGRAMS = kaffeh

GEN_SRCS = inflate.c jar.c utf8const.c readClass.c

DISTCLEANFILES = $(GEN_SRCS)

kaffeh_SOURCES = $(GEN_SRCS) constants.c sigs.c support.c main.c

noinst_HEADERS = constants.h file.h kaffeh-support.h readClassConfig.h \
	java_lang_ClassLoader.h \
	java_lang_Object.h \
	java_lang_String.h

kaffeh_LDADD = $(KAFFE_LIBS)

$(GEN_SRCS):
	$(SHELL) $(REGEN_FORWARD) $(srcdir)/../kaffevm/$@ $@

dist-hook:
	@for file in $(DISTCLEANFILES); do \
	  rm -f $(distdir)/$$file; \
	done
