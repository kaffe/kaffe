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

SUBDIRS = . po
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
	developers/patch-libtool-cdpath-zsh.patch \
	developers/patch-libtool-quote-sys_search_path.diff \
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
	echo BOOTCLASSPATH=\$${BOOTCLASSPATH}\''$(PATHSEP)'\'$(top_builddir)/libraries/javalib/rt.jar\''$(PATHSEP)'\'$(DNSJAVA_JAR)\'$(JAVAX_CRYPTO_JAR)\''$(PATHSEP)'\; export BOOTCLASSPATH | sed 's,/,$(DIRSEP),g;s,\\,\\\\,g' > BUILD_ENVIRONMENT.new; \
	echo CLASSPATH=.\''$(PATHSEP)'\'\$${GNU_CRYPTO_JAR}\''$(PATHSEP)'\'\$${JAVAX_SECURITY_JAR}\''$(PATHSEP)'\'$(top_srcdir)/libraries/javalib/kjc.jar\''$(PATHSEP)'\'$(BCEL_JAR)\''$(PATHSEP)'\'$(GNU_CRYPTO_JAR)\''$(PATHSEP)'\'$(JAVAX_SECURITY_JAR)\; export CLASSPATH | sed 's,/,$(DIRSEP),g;s,\\,\\\\,g' >> BUILD_ENVIRONMENT.new; \
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

# Build rt-precompiled.jar and add it to dist
dist-hook:
	rm -rf build-dist
	mkdir build-dist
	(cd build-dist && \
	 ../$(distdir)/configure --with-extensions=comm,microsoft,pjava,servlet,sound && \
	 make && \
	 cp libraries/javalib/rt.jar ../$(distdir)/libraries/javalib/rt-precompiled.jar \
	)

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


.PHONY: kaffe-build-order

kaffe-build-order:
	@list="$(DIRECTORIES_BEFORE_RTJAR)"; for subdir in $$list; do \
		(cd $$subdir && $(MAKE) $(KAFFE_BUILD_TARGET)) || exit $$? ; \
	done; \
	(cd libraries/javalib && $(MAKE) $(KAFFE_BUILD_TARGET)) || exit $$? ; \
	list="$(DIRECTORIES_AFTER_RTJAR)"; for subdir in $$list; do \
		(cd $$subdir && $(MAKE) $(KAFFE_BUILD_TARGET)) || exit $$? ; \
	done ;

all-local: KAFFE_BUILD_TARGET = all
all-local: kaffe-build-order
	cd kaffe/kaffe && $(MAKE) final-executable

install-exec-hook: KAFFE_BUILD_TARGET = install-exec
install-exec-hook: kaffe-build-order

install-data-hook: KAFFE_BUILD_TARGET = install-data
install-data-hook: kaffe-build-order

uninstall-hook: KAFFE_BUILD_TARGET = uninstall
uninstall-hook: kaffe-build-order

clean-local: KAFFE_BUILD_TARGET = clean
clean-local: kaffe-build-order

check-local:
	cd test && $(MAKE) check
