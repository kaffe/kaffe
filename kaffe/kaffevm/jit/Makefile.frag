# Makefile.frag for kaffevm JIT
#
# Copyright (c) 1996, 1997
#	Transvirtual Technologies, Inc.  All rights reserved.
#
# See the file "license.terms" for information on usage and redistribution 
# of this file. 

ENGINE_CFLAGS=	-DTRANSLATOR -I$(srcdir)/jit

ENGINE_OBJECTS=\
		basecode$(OBJEXT) \
		constpool$(OBJEXT) \
		funcs$(OBJEXT) \
		icode$(OBJEXT) \
		labels$(OBJEXT) \
		registers$(OBJEXT) \
		slots$(OBJEXT) \
		machine$(OBJEXT) \
		seq$(OBJEXT)

funcs.o:	funcs.c jit.def
machine.o:	machine.c kaffe.def
