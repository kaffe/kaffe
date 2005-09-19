# Makefile.in for libreplace - Replacement missing OS functions
#
# Copyright (c) 2002, 2003 University of Utah and the Flux Group.
# All rights reserved.
#
# This file is licensed under the terms of the GNU Public License.
# See the file "license.terms" for information on usage and redistribution
# of this file, and for a DISCLAIMER OF ALL WARRANTIES.
#
# Contributed by the Flux Research Group, Department of Computer Science,
# University of Utah, http://www.cs.utah.edu/flux/

AM_CPPFLAGS = -I$(top_builddir) -I$(top_srcdir) -I$(top_builddir)/include

noinst_LTLIBRARIES = libreplace.la

libreplace_la_LDFLAGS = \
        $(KVMLIBFLAGS) \
        -no-undefined

libreplace_la_SOURCES = \
	dummyin6.c \
	getaddrinfo.c \
	getifaddrs.c \
	inet_pton.c \
	inet_ntop.c \
	memset.c \
	repsemaphore.h \
	semaphores.c \
	ifaddrs.h

libreplace_la_LIBADD = \
        $(LTLIBINTL)

EXTRA_DIST = \
	ifaddrs_linux.c \
	ifaddrs_bsd.c
