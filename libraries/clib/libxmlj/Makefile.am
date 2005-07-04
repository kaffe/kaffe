#
# Copyright (C) 2003 Julian Scheid
#
# This file is part of GNU LibxmlJ, a JAXP-compliant Java wrapper for
# the XML and XSLT C libraries for Gnome (libxml2/libxslt).
#
# GNU LibxmlJ is free software; you can redistribute it and/or modify it
# under the terms of the GNU General Public License as published by the
# Free Software Foundation; either version 2, or (at your option) any
# later version.
#
# GNU LibxmlJ is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with GNU LibxmlJ; see the file COPYING.  If not, write to the
# Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
# 02110-1301 USA.
#

native_LTLIBRARIES = libxmlj.la

AM_CPPFLAGS = -I$(top_srcdir)/kaffe -I$(top_builddir)/include

libxmlj_la_SOURCES = \
	xmlj_dom.c \
	xmlj_dom.h \
	xmlj_error.c \
	xmlj_error.h \
	xmlj_io.c \
	xmlj_io.h \
	xmlj_node.c \
	xmlj_node.h \
	xmlj_sax.c \
	xmlj_sax.h \
	xmlj_transform.c \
	xmlj_util.c \
	xmlj_util.h \
	xmlj_xpath.c

libxmlj_la_CFLAGS = \
	-pedantic \
	-Wall \
	-fbounds-check \
        $(PTHREAD_CFLAGS) \
	$(CFLAGS_PG)

libxmlj_la_LDFLAGS = \
        $(KLIBFLAGS) \
        $(PTHREAD_LIBS) \
        -module \
	-export-dynamic \
        -release $(PACKAGE_VERSION)
