#
# s390/Linux configuration (clone from i386/linux/config.frag by ROSSP)
#
host_cpu=s390
host_os=linux

#if test x"$with_threads" = x"linux-threads" ; then
if test x"$with_threads" = x"unix-pthreads" ; then
	CPPFLAGS="$CPPFLAGS -D_REENTRANT"
	VM_LIBS="$VM_LIBS -lpthread"
fi

