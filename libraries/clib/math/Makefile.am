# Math support for standard Java libraries
#
# Copyright (c) 1996, 1997, 1998, 1999
#	Transvirtual Technologies, Inc.  All rights reserved.
#
# See the file "license.terms" for information on usage and redistribution 
# of this file. 

native_LTLIBRARIES = libmath.la

libmath_la_LDFLAGS = $(KLIBFLAGS) -module -release $(KVER)
libmath_la_LIBADD = $(MATH_LIBS) $(M_LIBS)
libmath_la_SOURCES = BigInteger.c

CLEANFILES = so_locations
