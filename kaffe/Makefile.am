# Base level Makefile.in for kaffe.
#
# Copyright (c) 1996, 1997, 1998, 1999
#	Transvirtual Technologies, Inc.  All rights reserved.
#
# See the file "license.terms" for information on usage and redistribution 
# of this file. 

if NEED_LIBXPROF
XPROF_SUBDIRS = xprof
endif

if ENABLE_JVMPI
JVMPI_SUBDIRS = jvmpi
endif

SUBDIRS = kaffeh kaffevm $(XPROF_SUBDIRS) $(JVMPI_SUBDIRS) kaffe scripts man
