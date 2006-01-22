# Process this with automake to create Makefile.in

ACLOCAL_AMFLAGS = -I m4

if COND_SYSTEM_ZLIB
LINKED_ZIP_LIBS=$(ZIP_LIBS)
else
LINKED_ZIP_LIBS=$(top_builddir)/../zlib/libz.la
MAYBE_INTERNAL_ZIP_INCLUDES=-I$(srcdir)/../zlib/
endif


AM_CPPFLAGS = \
	-I. \
	-I$(top_srcdir) \
	$(MAYBE_INTERNAL_ZIP_INCLUDES) \
	-I$(top_srcdir)/../include

bin_PROGRAMS = fastjar 
fastjar_SOURCES = jartool.c dostime.c compress.c pushback.c shift.c jartool.h \
zipfile.h dostime.h compress.h pushback.h shift.h 
fastjar_LDADD = $(LINKED_ZIP_LIBS) $(top_builddir)/../../../replace/libreplace.la
fastjar_CPPFLAGS = $(AM_CPPFLAGS) -DWITH_SHIFT_DOWN

AM_CFLAGS = \
	-I$(top_srcdir)/../../../replace \
	-I$(top_builddir)/../../../replace
AM_MAKEINFOFLAGS = -I $(srcdir)/../gcc/doc/include

TEXINFO_TEX = texinfo.tex
info_TEXINFOS = fastjar.texi
fastjar_TEXINFOS = \
	gcc-common.texi \
	gpl.texi
man_MANS = fastjar.1

TEXI2POD = perl $(srcdir)/../contrib/texi2pod.pl
POD2MAN = pod2man --center="GNU" --release="gcc-@gcc_version@"

.pod.1:
	-($(POD2MAN) --section=1 $< > $(@).T$$$$ && \
		mv -f $(@).T$$$$ $@) || \
		(rm -f $(@).T$$$$ && exit 1)

.INTERMEDIATE: fastjar.pod

fastjar.pod: $(srcdir)/fastjar.texi
	-$(TEXI2POD) -D fastjar $< > $@

all-local: stamp-geninsrc

stamp-geninsrc: fastjar.1 fastjar.info
	-cp -p $(top_builddir)/fastjar.1 $(srcdir)/fastjar.1
	-cp -p $(top_builddir)/fastjar.info $(srcdir)/fastjar.info
	touch $@

CLEANFILES = stamp-geninsrc
MAINTAINERCLEANFILES = $(srcdir)/fastjar.1 \
                       $(srcdir)/fastjar.info

DISTCLEANFILES = \
	config.log \
	config.status \
	install-defs.sh \
	config.h \
	stamp-h1

EXTRA_DIST = \
	$(man_MANS) \
	CHANGES \
	aclocal.m4 \
	config.h.in \
	configure \
	configure.ac \
	grepjar.1 \
	install-defs.sh.in \
	fastjar.info \
	m4/accross.m4 \
	m4/gcc_AC_FUNC_MKDIR_TAKES_ONE_ARG.m4 \
	scripts/compile \
	scripts/config.guess \
	scripts/config.sub \
	scripts/depcomp \
	scripts/install-sh \
	scripts/ltmain.sh \
	scripts/missing


