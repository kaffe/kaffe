# Makefile.in for kaffeh - kaffe native code stub generator.
#
# Copyright (c) 1996, 1997, 1998, 1999
#	Transvirtual Technologies, Inc.  All rights reserved.
#
# See the file "license.terms" for information on usage and redistribution 
# of this file. 

INCLUDES = -I../kaffevm -I$(srcdir)/../kaffevm -DKAFFEH

bin_PROGRAMS = kaffeh

BUILT_SOURCES = inflate.c jar.c utf8const.c readClass.c

kaffeh_SOURCES = constants.c sigs.c support.c main.c $(BUILT_SOURCES)

$(BUILT_SOURCES):
	$(SHELL) $(REGEN_FORWARD) $(srcdir)/../kaffevm/$@ $@
