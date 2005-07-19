
AM_CPPFLAGS = -I$(top_srcdir)/kaffe/kaffevm/$(THREAD_DIR) $(ENGINE_INCLUDES) -I$(top_srcdir)/libltdl $(ENGINE_DEFS) -I$(top_srcdir)/kaffe/kaffevm -I$(top_builddir)/include

lib_LTLIBRARIES = libkaffejvmpi.la

libkaffejvmpi_la_CFLAGS = \
	$(CFLAGS_PG)

libkaffejvmpi_la_LDFLAGS = \
	-no-undefined \
	-export-dynamic \
	-release $(PACKAGE_VERSION)

libkaffejvmpi_la_DEPENDENCIES = \
	$(LIBLTDL) \
	$(top_builddir)/kaffe/kaffevm/libkaffevm.la

libkaffejvmpi_la_LIBADD = \
	$(libkaffejvmpi_la_DEPENDENCIES) \
	$(KVMLIBFLAGS) \
	$(VM_LIBS) \
	$(M_LIBS)

libkaffejvmpi_la_SOURCES = \
	jvmpi_kaffe.c \
	jvmpi_kaffe.h
