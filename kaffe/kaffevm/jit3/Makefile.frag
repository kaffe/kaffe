# Makefile.frag for kaffevm JIT3
#
# Copyright (c) 1996, 1997, 1998, 1999
#	Transvirtual Technologies, Inc.  All rights reserved.
#
# See the file "license.terms" for information on usage and redistribution 
# of this file. 

ENGINE_INCLUDES = -Ijit3 -I$(srcdir)/jit3
ENGINE_DEFS = -DTRANSLATOR -DJIT3
