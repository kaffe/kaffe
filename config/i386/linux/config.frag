#
# i386/Linux configuration
#
host_cpu=i386
host_os=linux

if test x"$with_threads" = x"linux-threads" ; then
	CPPFLAGS="$CPPFLAGS -D_REENTRANT"
	VM_LIBS="$VM_LIBS -lpthread"
fi

