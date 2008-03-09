# Top-level Makefile for Kaffe Virtual Machine.
#
# Copyright (c) 1996, 1997, 1998, 1999
#	Transvirtual Technologies, Inc.  All rights reserved.
#
# Copyright (c) 2003
# 	Kaffe.org contributors. All rights reserved.
#
# See the file "license.terms" for information on usage and redistribution 
# of this file. 

## Tell ac local to pick up the m4 macros from m4 directory
ACLOCAL_AMFLAGS=-I m4

AUTOMAKE_OPTIONS = foreign 1.3e

SUBDIRS = \
	replace \
	libraries/javalib \
	config \
	kaffe/kaffeh \
	include  \
	kaffe/kaffevm \
	libraries/clib \
	kaffe \
	po 

DIST_SUBDIRS = \
	config \
	include \
	replace \
	kaffe \
	libraries \
	test \
	po

EXTRA_DIST = \
	ChangeLog.1 \
	ChangeLog.2 \
	ChangeLog.3 \
	ChangeLog.4 \
	ChangeLog.5 \
	ChangeLog.6 \
	ChangeLog.7 \
	ChangeLog.8 \
	ChangeLog.9 \
	ChangeLog.10 \
	ChangeLog.11 \
	ChangeLog.12 \
	ChangeLog.13 \
	ChangeLog.14 \
	ChangeLog.15 \
	ChangeLog.16 \
	ChangeLog.17 \
	ChangeLog.18 \
	ChangeLog.19 \
	ChangeLog.20 \
	THIRDPARTY \
	WHATSNEW \
	RELEASE-NOTES \
	autobuild.sh \
	license.terms \
	license-lesser.terms \
	FAQ/FAQ.arm \
	FAQ/FAQ.BeOS \
	FAQ/FAQ.Known-Bugs \
	FAQ/FAQ.amigaos \
	FAQ/FAQ.automake \
	FAQ/FAQ.benchmarking \
	FAQ/FAQ.blackfin \
	FAQ/FAQ.charsets \
	FAQ/FAQ.class-states \
	FAQ/FAQ.code-layout \
	FAQ/FAQ.coding-style \
	FAQ/FAQ.cross-compiling \
	FAQ/FAQ.debugging \
	FAQ/FAQ.depend \
	FAQ/FAQ.dmalloc \
	FAQ/FAQ.drops \
	FAQ/FAQ.embedded \
	FAQ/FAQ.feedback \
	FAQ/FAQ.freebsd \
	FAQ/FAQ.gcblock \
	FAQ/FAQ.gcj \
	FAQ/FAQ.gcstrategy \
	FAQ/FAQ.hpux \
	FAQ/FAQ.hotjava \
	FAQ/FAQ.install-root \
	FAQ/FAQ.jit3 \
	FAQ/FAQ.jsignal \
	FAQ/FAQ.kaffemd \
	FAQ/FAQ.libffi \
	FAQ/FAQ.libtool \
	FAQ/FAQ.linux \
	FAQ/FAQ.locks \
	FAQ/FAQ.mauve \
	FAQ/FAQ.nativemethods \
	FAQ/FAQ.ncr \
	FAQ/FAQ.platform-status \
	FAQ/FAQ.profiler \
	FAQ/FAQ.ps2linux \
	FAQ/FAQ.pthreads \
	FAQ/FAQ.requiredlibraries \
	FAQ/FAQ.staticnative \
	FAQ/FAQ.timing \
	FAQ/FAQ.unicode \
	FAQ/FAQ.win32 \
	FAQ/FAQ.xdebugging \
	FAQ/FAQ.xprofiler \
	FAQ/FAQ.references \
	compat-include/ansidecl.h \
	compat-include/eh-common.h \
	compat-include/frame.h \
	compat-include/gansidecl.h \
	developers/alignment_of_size.c \
	developers/auto-build.conf \
	developers/autogen.sh \
	developers/config1.patch \
	developers/gdbinit \
	developers/geteh_from_libgcc2 \
	developers/glibc-2.1.1-signal.patch \
	developers/fixup.c \
	developers/mangleNative.pl \
	developers/mnemonicizer.awk \
	developers/rpm-kaffe.spec \
	developers/sp_offset.c \
	developers/test-kaffe-sh \
	developers/FullTest.sh \
	developers/GCJ.note.1 \
	developers/README \
	scripts/GCCWarning.pm \
	scripts/JikesWarning.pm \
	scripts/LogWarning.pm \
	scripts/Registry.pm \
	scripts/sort-warnings.pl \
	scripts/SparseWarning.pm \
	scripts/WarningDescription.pm

CLEANFILES = BUILD_ENVIRONMENT

depend:
	@echo \`make depend\' is no longer needed

noinst_SCRIPTS = BUILD_ENVIRONMENT

BUILD_ENVIRONMENT: Makefile
	$(MAKE) top_srcdir=`cd $(top_srcdir) >/dev/null; pwd` \
		top_builddir=`cd $(top_builddir) >/dev/null; pwd` \
		"DEBUG_ENV=$(DEBUG_ENV)" BUILD_ENVIRONMENT-make

BUILD_ENVIRONMENT-make: Makefile
	echo BOOTCLASSPATH=$(top_builddir)/libraries/javalib/vmspecific/kaffe-vminterface.jar\''$(PATHSEP)'\'$(top_builddir)/libraries/javalib/zlib-zip/kaffe-zlib.jar\''$(PATHSEP)'\'$(GLIBJ_ZIP)\''$(PATHSEP)'\'\$${BOOTCLASSPATH}\''$(PATHSEP)'\'\; export BOOTCLASSPATH | $(SED) 's,/,$(DIRSEP),g;s,\\,\\\\,g' > BUILD_ENVIRONMENT.new; \
	echo CLASSPATH=.'$(PATHSEP)'\'$(BCEL_JAR)\''$(PATHSEP)'\; export CLASSPATH | $(SED) 's,/,$(DIRSEP),g;s,\\,\\\\,g' >> BUILD_ENVIRONMENT.new; \
	echo KAFFELIBRARYPATH=$(top_builddir)/kaffe/kaffevm/.libs'$(PATHSEP)'\'$(top_builddir)/libraries/clib/zip/.libs\''$(PATHSEP)'\'@CLASSPATH_LIBDIR@${DIRSEP}classpath\''$(PATHSEP)'\'.\'\; export KAFFELIBRARYPATH >> BUILD_ENVIRONMENT.new; \
	echo JAVA=$(top_builddir)/kaffe/kaffe/kaffe-bin$(EXEEXT)\; export JAVA >> BUILD_ENVIRONMENT.new; \
	rm -f BUILD_ENVIRONMENT
	mv BUILD_ENVIRONMENT.new BUILD_ENVIRONMENT

# Build HTML docs (requires maven and maven-sddocbook-plugin)
docs-docbook-html:
	maven sdocbook:generate-html

# Regenerate HTML docs and copy them to source tree
docs-regen: docs-docbook-html
	cp -f target/docs/docbook/*.html $(srcdir)/docs/html

check-local:
	cd test && $(MAKE) check

install-exec-local:
	$(MKDIR_P) $(DESTDIR)${toolslibdir}
	$(LN_S) -f $(CLASSPATH_PREFIX)/share/classpath/tools.zip $(DESTDIR)${toolslibdir}/tools.jar

uninstall-local:
	$(RM) $(DESTDIR)${toolslibdir}/tools.jar 
