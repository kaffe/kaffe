# Build the shared library support.
#
# Copyright (c) 1996, 1997, 1998, 1999
#	Transvirtual Technologies, Inc.  All rights reserved.
#
# Copyright (c) 2004
#       Kaffe.org contributors, see ChangeLogs for details.  All rights reserved.
#
# See the file "license.terms" for information on usage and redistribution 
# of this file. 

if COND_LIBXMLJ
  MAYBE_LIBXMLJ_DIR = libxmlj
endif

if COND_NATIVE_AWT
  MAYBE_NATIVE_AWT_DIR = awt
endif

if COND_NATIVE_SOUND
  MAYBE_NATIVE_SOUND_DIR = sound
endif

SUBDIRS = \
	target \
	classpath \
	fdlibm \
	native \
	io \
	nio \
	net \
	zip \
	math \
	security \
	management \
	$(MAYBE_LIBXMLJ_DIR) \
	$(MAYBE_NATIVE_AWT_DIR) \
	$(MAYBE_NATIVE_SOUND_DIR)
