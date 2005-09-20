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

# binreloc seems a bit painful to integrate, it may be unnecessary
# if we get turn the shared native libraries into convenience
# libraries.
#if ENABLE_BINRELOC
#MAYBE_BINRELOC_DIR = binreloc
#endif

SUBDIRS = \
	. \
	libraries/javalib \
	config \
	libltdl \
	$(MAYBE_BINRELOC_DIR) \
	kaffe/kaffeh \
	include  \
	replace \
	kaffe/kaffevm \
	libraries/clib \
	kaffe \
	po \
	tools

DIST_SUBDIRS = config include replace libltdl kaffe libraries tools test po

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
	THIRDPARTY \
	WHATSNEW \
	RELEASE-NOTES \
	license.terms \
	license-lesser.terms \
	license-w3c.terms \
	FAQ/FAQ.BeOS \
	FAQ/FAQ.Known-Bugs \
	FAQ/FAQ.amigaos \
	FAQ/FAQ.automake \
	FAQ/FAQ.awt \
	FAQ/FAQ.benchmarking \
	FAQ/FAQ.bignum \
	FAQ/FAQ.charsets \
	FAQ/FAQ.class-states \
	FAQ/FAQ.classlibrary-compile \
	FAQ/FAQ.code-layout \
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
	FAQ/FAQ.gnu-classpath \
	FAQ/FAQ.hpux \
	FAQ/FAQ.hotjava \
	FAQ/FAQ.install-root \
	FAQ/FAQ.jit3 \
	FAQ/FAQ.jsignal \
	FAQ/FAQ.kaffemd \
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
	FAQ/FAQ.references \
	compat-include/ansidecl.h \
	compat-include/eh-common.h \
	compat-include/frame.h \
	compat-include/gansidecl.h \
	binreloc/ChangeLog \
	binreloc/libtest.c \
	binreloc/Makefile \
	binreloc/prefix.c \
	binreloc/prefix.h \
	binreloc/README \
	binreloc/test.c \
	developers/boehm-with-kaffe.patch \
	developers/build_mathnamespace \
	developers/check-classpath-unmerged \
	developers/math_symbols \
	developers/check-classpath-merge-status \
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
	developers/mnemonicizer.awk \
	developers/resync-classpath.sh \
	developers/rpm-kaffe.spec \
	developers/sp_offset.c \
	developers/unicode.pl \
	developers/update-class-list \
	developers/utf8munge.pl \
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

noinst_SCRIPTS = libtool BUILD_ENVIRONMENT

libtool: $(LIBTOOL_DEPS)
	$(SHELL) ./config.status --recheck

BUILD_ENVIRONMENT: Makefile
	$(MAKE) top_srcdir=`cd $(top_srcdir) >/dev/null; pwd` \
		top_builddir=`cd $(top_builddir) >/dev/null; pwd` \
		"DEBUG_ENV=$(DEBUG_ENV)" BUILD_ENVIRONMENT-make

BUILD_ENVIRONMENT-make: Makefile
	echo BOOTCLASSPATH=\$${BOOTCLASSPATH}\''$(PATHSEP)'\'$(top_builddir)/libraries/javalib/rt.jar\''$(PATHSEP)'\'$(DNSJAVA_JAR)\''$(PATHSEP)'\'\; export BOOTCLASSPATH | sed 's,/,$(DIRSEP),g;s,\\,\\\\,g' > BUILD_ENVIRONMENT.new; \
	echo CLASSPATH=.\''$(PATHSEP)'\'\$${GNU_CRYPTO_JAR}\''$(PATHSEP)'\'\$${JAVAX_SECURITY_JAR}\''$(PATHSEP)'\'$(BCEL_JAR)\''$(PATHSEP)'\'$(GNU_CRYPTO_JAR)\''$(PATHSEP)'\'$(JAVAX_SECURITY_JAR)\; export CLASSPATH | sed 's,/,$(DIRSEP),g;s,\\,\\\\,g' >> BUILD_ENVIRONMENT.new; \
	echo KAFFELIBRARYPATH=\$${KAFFELIBRARYPATH+\"\$$KAFFELIBRARYPATH\"\''$(PATHSEP)'\'}$(top_builddir)/libraries/clib/native/.libs\''$(PATHSEP)'\'$(top_builddir)/libraries/clib/net/.libs\''$(PATHSEP)'\'$(top_builddir)/libraries/clib/io/.libs\''$(PATHSEP)'\'$(top_builddir)/libraries/clib/nio/.libs\''$(PATHSEP)'\'$(top_builddir)/libraries/clib/zip/.libs\''$(PATHSEP)'\'$(top_builddir)/libraries/clib/management/.libs\''$(PATHSEP)'\'$(top_builddir)/libraries/clib/security/.libs\''$(PATHSEP)'\'$(top_builddir)/libraries/clib/classpath/.libs\; export KAFFELIBRARYPATH >> BUILD_ENVIRONMENT.new; \
	echo JAVA=$(top_builddir)/kaffe/kaffe/kaffe-bin$(EXEEXT)\; export JAVA >> BUILD_ENVIRONMENT.new; \
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

# Build HTML docs (requires maven and maven-sddocbook-plugin)
docs-docbook-html:
	maven sdocbook:generate-html

# Regenerate HTML docs and copy them to source tree
docs-regen: docs-docbook-html
	cp -f target/docs/docbook/*.html $(srcdir)/docs/html


#
# things we need to compile before we can compile rt.jar 
#
DIRECTORIES_BEFORE_RTJAR = \
	config \
	include \
	replace \
	libltdl \
	kaffe/kaffevm \
	libraries/clib \
	kaffe

#
# things we need to compile after we have compiled rt.jar 
#
DIRECTORIES_AFTER_RTJAR = \
	tools \
	test \
	$(RUN_PO)

if MAY_RUN_PO
RUN_PO=po
else
RUN_PO=
endif

if COND_LIBXMLJ
DIRECTORIES_AFTER_RTJAR += libraries/clib/libxmlj
endif

if COND_SOUND_ALSA
DIRECTORIES_AFTER_RTJAR += libraries/clib/sound
else 
if COND_SOUND_ESD
DIRECTORIES_AFTER_RTJAR += libraries/clib/sound
endif
endif

if COND_NATIVE_AWT
if COND_KAFFE_X_AWT
DIRECTORIES_AFTER_RTJAR += libraries/clib/awt/X
endif
if COND_KAFFE_QT_AWT
DIRECTORIES_AFTER_RTJAR += libraries/clib/awt/qt
endif
if COND_CLASSPATH_GTK_AWT
DIRECTORIES_AFTER_RTJAR += libraries/clib/awt/classpath-gtk
endif
endif

check-local:
	cd test && $(MAKE) check
