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

# Build libxmlj only desired
if COND_LIBXMLJ
MAYBE_LIBXMLJ = libxmlj
else
MAYBE_LIBXMLJ =
endif

if COND_NATIVE_AWT
MAYBE_AWT = awt
else
MAYBE_AWT =
endif

SUBDIRS = native io nio net zip math security management $(MAYBE_LIBXMLJ) $(MAYBE_AWT)

DIST_SUBDIRS = native io nio net zip math libxmlj security management awt sound
