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

Kaffe_LDADD_ = ../kaffevm/libkaffevm.la

Kaffe_LDFLAGS = $(LDFLAGS) -export-dynamic $(Kaffe_LDADD_) \
	`for lib in $(JAVA_LIBS); do echo -dlopen $$lib; done`

Kaffe_DEPENDENCIES = $(Kaffe_LDADD_) $(JAVA_LIBS)
