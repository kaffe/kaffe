# Makefile.am for the garbage collector - original Kaffe's GC.
#
# Copyright (c) 2004
#	Kaffe.org contributors, see ChangeLogs for details.  All rights reserved.
#
# See the file "license.terms" for information on usage and redistribution 
# of this file. 

AM_CPPFLAGS = \
        -I$(top_srcdir)/include \
        -I$(top_builddir)/config \
        -I$(top_srcdir)/config \
	-I$(top_srcdir)/kaffe/kaffevm/$(THREAD_DIR) \
	-I$(top_builddir)/include \
	-I$(top_srcdir)/kaffe/kaffevm \
	-I$(top_srcdir)/kaffe/jvmpi \
	-I$(top_srcdir)/kaffe/kaffevm/boehm-gc/boehm/include \
	$(BOEHMGC_SPECIFIC_FLAGS)

noinst_PROGRAMS = if_mach

if_mach_SOURCES= boehm/if_mach.c

native_LTLIBRARIES = libkaffegc.la

libkaffegc_la_LDFLAGS = \
	$(KVMLIBFLAGS) \
	-export-dynamic \
	-release $(PACKAGE_VERSION)

libkaffegc_la_SOURCES= \
	gc2.c \
	\
	boehm/allchblk.c boehm/alloc.c boehm/blacklst.c boehm/checksums.c \
	boehm/dbg_mlc.c boehm/dyn_load.c boehm/finalize.c \
	boehm/include/private/gcconfig.h \
	boehm/include/gc.h \
	boehm/include/gc_alloc.h \
	boehm/include/gc_cpp.h \
	boehm/include/private/gc_hdrs.h \
	boehm/include/private/gc_mark.h \
	boehm/include/private/gc_priv.h \
	boehm/include/gc_typed.h \
	boehm/gcj_mlc.c \
	boehm/headers.c \
	boehm/aix_irix_threads.c \
	boehm/win32_threads.c \
	boehm/solaris_pthreads.c \
	boehm/solaris_threads.c \
	boehm/pthread_support.c \
	boehm/malloc.c \
	boehm/mallocx.c \
	boehm/mark.c \
	boehm/mark_rts.c \
	boehm/misc.c \
	boehm/new_hblk.c \
	boehm/obj_map.c \
	boehm/os_dep.c \
	boehm/pcr_interface.c \
	boehm/ptr_chck.c \
	boehm/real_malloc.c \
	boehm/reclaim.c \
	boehm/stubborn.c \
	boehm/typd_mlc.c \
	boehm/mach_dep.c \
	boehm/version.h \
	boehm/include/private/solaris_threads.h \
	boehm/include/weakpointer.h

EXTRA_DIST= boehm
