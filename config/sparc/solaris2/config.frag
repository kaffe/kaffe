#
# Sparc/Solaris2 configuration
#
host_cpu=sparc
host_os=solaris2
if test "$ac_cv_prog_gcc" = "yes" ; then
	PIC=-fPIC
else
	PIC=-KPIC
	CFLAGS="-g -DUSE_SPARC_PROF_C"
fi
LIBSHARE=-G  
LIBEXT=.so 
