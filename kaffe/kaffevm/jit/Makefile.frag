# Makefile.frag for kaffevm JIT
#
# Copyright (c) 1996, 1997
#	Transvirtual Technologies, Inc.  All rights reserved.
#
# See the file "license.terms" for information on usage and redistribution 
# of this file. 

ENGINE_INCLUDES = -I$(top_builddir)/kaffe/kaffevm/jit -I$(top_srcdir)/kaffe/kaffevm/jit
ENGINE_DEFS = -DTRANSLATOR
