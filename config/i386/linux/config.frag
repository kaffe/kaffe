#
# i386/Linux configuration
#
host_cpu=i386
host_os=linux
if test "$orig_host_os" = "linuxoldld" ; then
	dynamic_libraries=no
else
	PIC=-fPIC
	LIBSHARE=-shared
	LIBEXT=.so
	# Uncomment the following two lines to make the VM part of
	# the invocation program (kaffe).  This is necessary for debugging
	# on some systems (notable Redhat 5.0).
	# KAFFELINK="-Wl,--export-dynamic"
	# vm_dynamic_library=no
fi
