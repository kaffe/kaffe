AM_CPPFLAGS = \
	-I$(top_srcdir)/kaffe/kaffevm/$(THREAD_DIR) \
	$(ENGINE_INCLUDES) \
	$(ENGINE_DEFS) \
	-I$(top_srcdir)/kaffe/kaffevm \
	-I$(top_builddir)/include \
        -I$(top_srcdir)/config/$(Khost_cpu) \
        -I$(top_srcdir)/config/$(Khost_cpu)/$(Khost_os)

lib_LTLIBRARIES = libkaffejvmpi.la

libkaffejvmpi_la_CFLAGS = \
	$(CFLAGS_PG)

libkaffejvmpi_la_LDFLAGS = \
	-no-undefined \
	-export-dynamic \
	-release $(PACKAGE_VERSION)

libkaffejvmpi_la_DEPENDENCIES = \
	$(top_builddir)/kaffe/kaffevm/libkaffe.la

libkaffejvmpi_la_LIBADD = \
	$(libkaffejvmpi_la_DEPENDENCIES) \
	$(KVMLIBFLAGS) \
	$(VM_LIBS) \
	$(M_LIBS)

libkaffejvmpi_la_SOURCES = \
	jvmpi_kaffe.c \
	jvmpi_kaffe.h
