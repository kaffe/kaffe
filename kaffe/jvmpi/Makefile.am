
AM_CPPFLAGS = -I$(top_srcdir)/kaffe/kaffevm/$(THREAD_DIR) $(ENGINE_INCLUDES) -I$(top_srcdir)/libltdl $(ENGINE_DEFS) -I$(top_srcdir)/kaffe/kaffevm -I$(top_builddir)/include

lib_LTLIBRARIES = libkaffejvmpi.la

libkaffejvmpi_la_LDFLAGS = -export-dynamic -release $(KVER)
libkaffejvmpi_la_DEPENDENCIES = $(LIBLTDL)
libkaffejvmpi_la_LIBADD = $(libkaffejvmpi_la_DEPENDENCIES) $(VM_LIBS) $(M_LIBS)

libkaffejvmpi_la_SOURCES = \
	jvmpi_kaffe.c \
	jvmpi_kaffe.h
