# A brief demonstration of using Automake with Libtool. -*-Makefile-*-
#
AUTOMAKE_OPTIONS = no-dependencies foreign

LTDL_VERSION = -version-info 1:0:1

if INSTALL_LTDL
LTDL_FLAGS = $(LTDL_VERSION) -rpath $(libdir) 
include_HEADERS = ltdl.h
else
LTDL_FLAGS =
noinst_HEADERS = ltdl.h
endif

EXTRA_LTLIBRARIES = libltdl.la libltdlc.la

lib_LTLIBRARIES = @LIBLIBS@
noinst_LTLIBRARIES = @NOINSTLIBS@

libltdl_la_SOURCES = ltdl.c
libltdl_la_LDFLAGS = $(LTDL_FLAGS)
libltdl_la_LIBADD = $(LIBADD_DL)
libltdl_la_DEPENDENCIES = libtool

libltdlc_la_SOURCES = $(libltdl_la_SOURCES)
libltdlc_la_LDFLAGS =
libltdlc_la_LIBADD = $(libltdl_la_LIBADD)
libltdlc_la_DEPENDENCIES = libtool

libtool: ../libtool
	./config.status --recheck
