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

if COND_NATIVE_AWT
  MAYBE_NATIVE_AWT_DIR = awt
endif

if COND_PURE_JAVA_MATH
else
  MAYBE_NATIVE_MATH_DIR = math
endif

if COND_ZLIB_ZIP
  MAYBE_ZLIB_ZIP = zip
endif

SUBDIRS = \
	$(MAYBE_NATIVE_AWT_DIR) \
	$(MAYBE_NATIVE_MATH_DIR) \
	$(MAYBE_ZLIB_ZIP)
