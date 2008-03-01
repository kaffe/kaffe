# Makefile.in for kaffeh - kaffe native code stub generator.
#
# Copyright (c) 1996, 1997, 1998, 1999
#	Transvirtual Technologies, Inc.  All rights reserved.
#
# See the file "license.terms" for information on usage and redistribution 
# of this file. 

AM_CFLAGS= \
	$(CFLAGS_PG)

AM_CPPFLAGS = \
	-I../kaffevm \
	-I$(srcdir)/../kaffevm \
	-DKAFFEH \
	-I$(top_builddir)/include \
	-I$(top_builddir)/config \
	-I$(top_srcdir)/kaffe/kaffevm/jni \
	-I$(top_srcdir)/config/$(Khost_cpu) \
	-I$(top_srcdir)/config/$(Khost_cpu)/$(Khost_os) \
	$(ZZIP_CFLAGS)

bin_PROGRAMS = kaffeh

kaffeh_LDADD = \
	$(LTLIBINTL) \
	$(ZZIP_LIBS) 


kaffeh_SOURCES = \
	main.c \
	mem.c \
	sigs.c \
	support.c \
	kaffeh-support.h \
	java_lang_ClassLoader.h \
	java_lang_Object.h \
	java_lang_String.h \
	$(top_srcdir)/kaffe/kaffevm/constants.c \
	$(top_srcdir)/kaffe/kaffevm/debug.c \
	$(top_srcdir)/kaffe/kaffevm/file.c \
	$(top_srcdir)/kaffe/kaffevm/jar.c \
	$(top_srcdir)/kaffe/kaffevm/readClass.c \
	$(top_srcdir)/kaffe/kaffevm/utf8const.c 

