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

kaffeh_SOURCES = $(GEN_SRCS) constants.c sigs.c support.c main.c

noinst_HEADERS = constants.h file.h kaffeh-support.h readClassConfig.h

kaffeh_LDADD = $(KAFFE_LIBS)

$(GEN_SRCS):
	$(SHELL) $(REGEN_FORWARD) $(srcdir)/../kaffevm/$@ $@

all: $(KAFFEH_build)

.PHONY: kaffeh-build
kaffeh-build:
	@test -d ../kaffeh-build || mkdir ../kaffeh-build
	cd ../kaffeh-build && $(MAKE) -f ../kaffeh/Makefile kaffeh \
		"CC=$(BUILD_CC)" "srcdir=$(srcdir)/../kaffeh"
