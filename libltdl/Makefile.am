# A brief demonstration of using Automake with Libtool. -*-Makefile-*-
#
AUTOMAKE_OPTIONS = no-dependencies foreign

LTDL_VERSION = -version-info 1:1:1

if INSTALL_LTDL
include_HEADERS = ltdl.h
else
noinst_HEADERS = ltdl.h
endif

noinst_LTLIBRARIES = libltdl.la

libltdl_la_SOURCES = ltdl.c
libltdl_la_LIBADD = $(LIBADD_DL)

lib_LTLIBRARIES = @LIBLIBS@
EXTRA_LTLIBRARIES =

@TOINST@/libltdl.la: $(libltdl_la_OBJECTS) $(libltdl_la_DEPENDENCIES)
	@test -d $(TOINST) || mkdir $(TOINST)
	$(LINK) -o $@ $(libltdl_la_OBJECTS) \
		-rpath $(libdir) $(LTDL_VERSION) $(libltdl_la_LIBADD)

distclean-local:
	test -z $(TOINST) || rm -rf $(TOINST)

libtool: $(LIBTOOL_DEPS)
	$(SHELL) ./config.status --recheck
Makefile: libtool
