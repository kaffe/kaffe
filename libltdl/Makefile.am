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

EXTRA_LTLIBRARIES = libltdl.la

lib_LTLIBRARIES = @LIBLIBS@
noinst_LTLIBRARIES = @NOINSTLIBS@

libltdl_la_SOURCES = ltdl.c
libltdl_la_LDFLAGS = $(LTDL_FLAGS) $(LIBADD_DL)
