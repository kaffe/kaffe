# Makefile.in for kaffe - a Java(tm) compatible virtual machine wrapper.
#
# Copyright (c) 1996, 1997, 1998, 1999
#	Transvirtual Technologies, Inc.  All rights reserved.
#
# See the file "license.terms" for information on usage and redistribution 
# of this file. 

libexec_PROGRAMS = Kaffe

INCLUDES = -I../kaffevm -I$(srcdir)/../kaffevm -I$(top_srcdir)/libltdl

Kaffe_SOURCES = main.c

LIBKAFFEVM = ../kaffevm/libkaffevm.la

Kaffe_LDFLAGS = -export-dynamic
Kaffe_LDADD = $(LIBKAFFEVM) $(DLOPEN_JAVA_LIBS) $(KAFFE_LIBS)
Kaffe_DEPENDENCIES = $(LIBKAFFEVM) $(JAVA_LIBS)
