# Math support for standard Java libraries
#
# Copyright (c) 1996, 1997, 1998, 1999
#	Transvirtual Technologies, Inc.  All rights reserved.
#
# See the file "license.terms" for information on usage and redistribution 
# of this file. 

native_LTLIBRARIES = libsecurity.la

SECURITY_SRCS =	\
		digest.c

# XXX                                                               XXX
# XXX     Need to create install directory ${prefix}/lib/security   XXX
# XXX     and install java.security into it ...                     XXX
# XXX                                                               XXX

libsecurity_la_LDFLAGS = $(KLIBFLAGS) -module -release $(KVER) -export-symbols-regex "^([Jj]ava|kaffe)_"
libsecurity_la_LIBADD = $(SECURITY_LIBS)
libsecurity_la_SOURCES = $(SECURITY_SRCS)

CLEANFILES = so_locations
