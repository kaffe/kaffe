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

#
# what a mess this is
#
#if [ "$oskit_env" = "real" ]
#then
#KAFFE_LIBS="\
#   -loskit_startup  -loskit_clientos		\
#   -loskit_threads  -loskit_svm -loskit_amm	\
#   -loskit_bootp				\
#   -loskit_freebsd_net				\
#   -loskit_linux_dev -loskit_dev		\
#   -loskit_netbsd_fs				\
#   -loskit_kern					\
#   -loskit_lmm					\
#   -loskit_diskpart -loskit_memfs		\
#   -loskit_freebsd_c_r  -loskit_fsnamespace_r \
#   -loskit_com		\
#   -loskit_freebsd_m -loskit_freebsd_c_r	\
#   -loskit_threads	\
#$KAFFE_LIBS "
#elif (exit 0)
#then
# Some things we want to link with aren't in the oskit lib directory, and
# some of them aren't even archive.  What to do?
#KAFFE_LIBS="\
#        ${oskit}/lib/unix_support_pthreads.o			\
#        -loskit_startup  -loskit_clientos -loskit_fsnamespace_r	\
#        -loskit_threads -loskit_unix -loskit_dev		\
#  	-loskit_freebsd_c_r  -loskit_com			\
#  	-loskit_freebsd_m -loskit_freebsd_c_r			\
#        ${oskit}/examples/unix/freebsd/libfreebsdsys.a		\
#        -loskit_threads						\
#$KAFFE_LIBS "
#fi
#
