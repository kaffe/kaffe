#
# Copyright (c) 1998, 1999, 2000 The University of Utah. All rights reserved.
#
# See the file "license.terms" for information on usage and redistribution
# of this file.
#
# Contributed by the Flux Research Group at the University of Utah.
# Authors: Godmar Back, Leigh Stoller
#

echo "************************************************************"
echo "Using OSKit config.frag"
echo "************************************************************"

# Always cross-compiling.  We have to do this because when
# building for OSKit/UNIX the simple test program actually
# builds and runs!
ac_cv_prog_cc_cross='yes'
cross_compiling='yes'

#
# Specify various x86 specific constants that a cross-compiler cannot
# figure out.
#
ac_cv_c_bigendian='no'
ac_cv_sizeof_voidp='4'
ac_cv_sizeof_long='4'
ac_cv_sizeof_short='2'
ac_cv_sizeof_long_long='8'
ac_cv_sizeof_int='4'
ac_cv_alignmentof_voidp='4'

ac_cv_strtod_m0_broken='yes' 
ac_cv_long_long_modulo_broken='yes'

# oskit libc's depends on osenv_wakeup, so it doesn't link XXX
ac_cv_func_select='yes'

ac_cv_lib_md_MD5Init='no'
# autoconf tends to erroneously fall back to /lib/cpp
CPP="$CC -E"

dynamic_libraries=no
 
# as far as Kaffe is concerned, we have these XXX
# we might actually have them meanwhile ???
ac_cv_func_sigemptyset='yes'
ac_cv_func_sigaddset='yes'
ac_cv_func_sigprocmask='yes'

ac_cv_func_dlopen='no'
ac_cv_func_dlerror='no'
ac_cv_func_sbrk='no'
ac_cv_func_getuid='no'

# Configure tries to compile *and* run a program to figure this out!
ac_cv_func_mmap_fixed_mapped=yes

#
# For the gc-block-mmap stuff, predefine the address range since
# otherwise its going to call sbrk(0), which has no meaning in the oskit.
#

#
# check whether OSKIT_UNIX is defined
#
cat <<END >conftest.c
#ifdef OSKIT_UNIX
#error oskit_env=unixsim
#else
#error oskit_env=real
#endif
END
# evaluate the statement after "conftest.c:2: #error oskit_env=unixsim"
eval `$CPP conftest.c 2>&1 | grep oskit_env | awk '{print $3}'`

