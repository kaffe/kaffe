# Build the shared library support.
#
# Copyright (c) 1996, 1997, 1998, 1999
#	Transvirtual Technologies, Inc.  All rights reserved.
#
# See the file "license.terms" for information on usage and redistribution 
# of this file. 

# Build math library only if native implementation of java.math should be used.
if COND_PURE_JAVA_MATH
  MAYBE_NATIVE_MATH =
else
  MAYBE_NATIVE_MATH = math
endif

SUBDIRS = native io net zip $(MAYBE_NATIVE_MATH) security management awt
