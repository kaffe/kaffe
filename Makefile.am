# Top-level Makefile for Kaffe OpenVM.
#
# Copyright (c) 1996, 1997, 1998, 1999
#	Transvirtual Technologies, Inc.  All rights reserved.
#
# See the file "license.terms" for information on usage and redistribution 
# of this file. 

AUTOMAKE_OPTIONS = foreign 1.3e

SUBDIRS = . config include libltdl libraries kaffe test

EXTRA_DIST = \
	ChangeLog.1 \
	WHATSNEW \
	license.terms \
	FAQ/FAQ.BeOS \
	FAQ/FAQ.Known-Bugs \
	FAQ/FAQ.amigaos \
	FAQ/FAQ.automake \
	FAQ/FAQ.awt \
	FAQ/FAQ.class-states \
	FAQ/FAQ.depend \
	FAQ/FAQ.gcblock \
	FAQ/FAQ.gcstrategy \
	FAQ/FAQ.hotjava \
	FAQ/FAQ.install-root \
	FAQ/FAQ.jsignal \
	FAQ/FAQ.libtool \
	FAQ/FAQ.linux \
	FAQ/FAQ.nativemethods \
	FAQ/FAQ.requiredlibraries \
	FAQ/FAQ.timing \
	FAQ/FAQ.win32 \
	developers/JavaClass.pm \
	developers/README \
	developers/dumpClass.pl \
	developers/gdbinit \
	developers/sp_offset.c \
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
	echo CLASSPATH=\$${CLASSPATH-.}\''$(PATHSEP)'\'$(top_srcdir)/libraries/javalib/Klasses.jar\''$(PATHSEP)'\'$(top_srcdir)/libraries/javalib/pizza.jar\; export CLASSPATH > BUILD_ENVIRONMENT; \
	echo KAFFELIBRARYPATH=\$${KAFFELIBRARYPATH+\"\$$KAFFELIBRARYPATH\"\''$(PATHSEP)'\'}`for f in $(JAVA_LIBS); do echo "$$f" | sed 's%/[^/]*$$%%'; done | (tr '\012' '$(PATHSEP)'; echo) | sed s/.$$//`\; export KAFFELIBRARYPATH >> BUILD_ENVIRONMENT; \
	echo JAVA=$(top_builddir)/kaffe/kaffe/Kaffe$(EXEEXT)\; export JAVA >> BUILD_ENVIRONMENT

.PHONY: Klasses new-classes compile-classes jar-classes build-classes
Klasses new-classes compile-classes jar-classes build-classes:
	cd libraries/javalib && $(MAKE) $@
