#
# m68k/Linux configuration.
#
host_cpu=m68k
host_os=linux
PIC=-fPIC
CFLAGS="-g -O -fno-omit-frame-pointer"
# if linux version is less that 2.0 we need to use static libraries
if [ `uname -r | cut -d. -f1` -lt 2 ]
then
	dynamic_libraries=no
else
	LIBSHARE=-shared
	LIBEXT=.so
fi
