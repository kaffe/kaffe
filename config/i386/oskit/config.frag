#
# Copyright (c) 1998 The University of Utah. All rights reserved.
#
# See the file "license.terms" for information on usage and redistribution
# of this file.
#
# Contributed by the Flux Research Group at the University of Utah.
# Authors: Godmar Back, Leigh Stoller
#

echo "************************************************************"
echo "Using OS Kit installation "
echo "************************************************************"

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

# oskit libc's depends on osenv_wakeup, so it doesn't link XXX
ac_cv_func_select='yes'

# autoconf tends to erroneously fall back to /lib/cpp
CPP="$CC -E"

dynamic_libraries=no

# as far as Kaffe is concerned, we have these XXX
# we might actually have them meanwhile ???
ac_cv_func_sigemptyset='yes'
ac_cv_func_sigaddset='yes'
ac_cv_func_sigprocmask='yes'

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

#
# what a mess this is
#
if [ "$oskit_env" = "real" ]
then
KAFFE_LIBS="\
                -loskit_startup \
                -loskit_threads \
                -loskit_svm -loskit_amm \
                -loskit_bootp \
                -loskit_freebsd_net \
                -loskit_linux_dev -loskit_dev \
                -loskit_kern -loskit_lmm \
		-loskit_netbsd_fs \
		-loskit_freebsd_c_r \
                -loskit_diskpart -loskit_fs -loskit_com \
                -loskit_threads \
$KAFFE_LIBS "
else
KAFFE_LIBS="\
                -loskit_startup \
                -loskit_threads \
                -loskit_bootp \
                -loskit_freebsd_net \
                -loskit_linux_dev -loskit_freebsd_c_r \
		-loskit_dev \
                -loskit_kern -loskit_lmm \
		-loskit_netbsd_fs \
		-loskit_freebsd_c_r \
                -loskit_diskpart -loskit_fs -loskit_com \
                -loskit_threads \
$KAFFE_LIBS "
fi

