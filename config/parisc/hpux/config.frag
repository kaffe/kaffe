#
# Parisc/HPUX configuration.
#
host_cpu=parisc
host_os=hpux
if test "$ac_cv_prog_gcc" = "yes" ; then
	PIC=-fpic
	LIBEXT=.sl
	LIBSHARE=-b
	INSTALL_DATA='${INSTALL} -m 555'
else
	# for the HP-UX c compiler only
	dynamic_libraries=no
	CFLAGS="-g -Aa +e -Dunix -Dhpux -D_HPUX_SOURCE"
fi
