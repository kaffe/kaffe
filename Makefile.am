# Top-level Makefile for Kaffe OpenVM.
#
# Copyright (c) 1996, 1997, 1998, 1999
#	Transvirtual Technologies, Inc.  All rights reserved.
#
# See the file "license.terms" for information on usage and redistribution 
# of this file. 

AUTOMAKE_OPTIONS = foreign 1.3e

SUBDIRS = . config include replace libltdl libraries/clib libraries/extensions kaffe libraries/javalib test

DIST_SUBDIRS = . config include replace libltdl kaffe libraries test

EXTRA_DIST = \
	ChangeLog.1 \
	ChangeLog.2 \
	ChangeLog.3 \
	ChangeLog.4 \
	THIRDPARTY \
	WHATSNEW \
	RELEASE-NOTES \
	license.terms \
	license-lesser.terms \
	license-w3c.terms \
	gwqt.m4 \
	iconv.m4 \
	libtool.m4 \
	FAQ/FAQ.BeOS \
	FAQ/FAQ.Known-Bugs \
	FAQ/FAQ.amigaos \
	FAQ/FAQ.automake \
	FAQ/FAQ.awt \
	FAQ/FAQ.benchmarking \
	FAQ/FAQ.class-states \
	FAQ/FAQ.classlibrary-compile \
	FAQ/FAQ.coding-style \
	FAQ/FAQ.cross-compiling \
	FAQ/FAQ.debugging \
	FAQ/FAQ.depend \
	FAQ/FAQ.dmalloc \
	FAQ/FAQ.dns \
	FAQ/FAQ.embedded \
	FAQ/FAQ.feedback \
	FAQ/FAQ.gcblock \
	FAQ/FAQ.gcj \
	FAQ/FAQ.gcstrategy \
	FAQ/FAQ.hotjava \
	FAQ/FAQ.install-root \
	FAQ/FAQ.jit3 \
	FAQ/FAQ.jsignal \
	FAQ/FAQ.kjc \
	FAQ/FAQ.libffi \
	FAQ/FAQ.libtool \
	FAQ/FAQ.linux \
	FAQ/FAQ.locks \
	FAQ/FAQ.mauve \
	FAQ/FAQ.nativemethods \
	FAQ/FAQ.ncr \
	FAQ/FAQ.profiler \
	FAQ/FAQ.ps2linux \
	FAQ/FAQ.pthreads \
	FAQ/FAQ.requiredlibraries \
	FAQ/FAQ.sound \
	FAQ/FAQ.staticnative \
	FAQ/FAQ.timing \
	FAQ/FAQ.unicode \
	FAQ/FAQ.win32 \
	FAQ/FAQ.xdebugging \
	FAQ/FAQ.xprofiler \
	compat-include/ansidecl.h \
	compat-include/eh-common.h \
	compat-include/frame.h \
	compat-include/gansidecl.h \
	developers/check-classpath-merge-status \
	developers/Encode.java \
	developers/EncodeEUC_JP.java \
	developers/FullTest.sh \
	developers/GCJ.note.1 \
	developers/JavaClass.pm \
	developers/README \
	developers/README.unicode \
	developers/README.EUC_JP \
	developers/alignment_of_size.c \
	developers/autogen.sh \
	developers/createLdScript.pl \
	developers/dumpClass.pl \
	developers/gdbinit \
	developers/fixup.c \
	developers/geteh_from_libgcc2 \
	developers/glibc-2.1.1-signal.patch \
	developers/mangleNative.pl \
	developers/mauve-html-gen.pl \
	developers/mauve-kaffe \
	developers/mauve-results.sh \
	developers/patch-config.sub-superh.diff \
	developers/patch-libtool-amiga.diff \
	developers/patch-libtool-openbsd.diff \
	developers/patch-libtool-realloc.diff \
	developers/rpm-kaffe.spec \
	developers/sp_offset.c \
	developers/unicode.pl \
	developers/update-class-list \
	developers/utf8munge.pl

CLEANFILES = BUILD_ENVIRONMENT

depend:
	@echo \`make depend\' is no longer needed

noinst_SCRIPTS = libtool BUILD_ENVIRONMENT

libtool: $(LIBTOOL_DEPS)
	$(SHELL) ./config.status --recheck

BUILD_ENVIRONMENT: Makefile
	$(MAKE) top_srcdir=`cd $(top_srcdir) >/dev/null; pwd` \
		top_builddir=`cd $(top_builddir) >/dev/null; pwd` \
		"DEBUG_ENV=$(DEBUG_ENV)" BUILD_ENVIRONMENT-make

BUILD_ENVIRONMENT-make: Makefile
	echo BOOTCLASSPATH=\$${BOOTCLASSPATH}\''$(PATHSEP)'\'$(top_builddir)/libraries/javalib/rt.jar\''$(PATHSEP)'\'$(DNSJAVA_JAR)\; export BOOTCLASSPATH | sed 's,/,$(DIRSEP),g;s,\\,\\\\,g' > BUILD_ENVIRONMENT.new; \
	echo CLASSPATH=\$${CLASSPATH}\''$(PATHSEP)'\'.\''$(PATHSEP)'\'$(top_srcdir)/libraries/javalib/kjc.jar\; export CLASSPATH | sed 's,/,$(DIRSEP),g;s,\\,\\\\,g' >> BUILD_ENVIRONMENT.new; \
	echo KAFFELIBRARYPATH=\$${KAFFELIBRARYPATH+\"\$$KAFFELIBRARYPATH\"\''$(PATHSEP)'\'}`for f in $(JAVA_LIBS); do echo "$$f" | sed 's%/[^/]*$$%%'; done | (tr '\012' ' '; echo) | sed -e 's/ $$//' -e "s/ /\'$(PATHSEP)\'/g"`\; export KAFFELIBRARYPATH >> BUILD_ENVIRONMENT.new; \
	echo JAVA=$(top_builddir)/kaffe/kaffe/kaffe-bin$(EXEEXT)\; export JAVA >> BUILD_ENVIRONMENT.new
	rm -f BUILD_ENVIRONMENT
	mv BUILD_ENVIRONMENT.new BUILD_ENVIRONMENT

CLASSDIRS = libraries/javalib

.PHONY: Klasses new-classes compile-classes jar-classes build-classes
Klasses new-classes compile-classes jar-classes build-classes:
	@if test "$(CLASSDIRS)" = all; then \
	   $(MAKE) CLASSDIRS="`echo libraries/javalib \
				    libraries/extensions/*/javalib`" $@; \
	else \
	  for f in $(CLASSDIRS); do \
	    (cd $$f && $(MAKE) $@); \
	  done; \
	fi
