#
# i386/Linux configuration
#
Khost_cpu=i386
Khost_os=linux

if test x"$with_threads" = x"linux-threads" ; then
	CPPFLAGS="$CPPFLAGS -D_REENTRANT"
	VM_LIBS="$VM_LIBS -lpthread"
fi

