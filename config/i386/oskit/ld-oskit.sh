#!/bin/sh
#
# ld-oskit.sh
#
# This is a magic wrapper for the link step in Kaffe.  It 
# supplies all of the necessary OSKit libraries (of which
# there are many all of which have odd ordering dependencies).
#
# The --oskit=* option is set by the oskit-configure shell script.
#
# XXX could use a --bootloader=foo option
#
# XXX Instead of groking the magical (and useless -DOSKIT_UNIX),
# should take a --unixmode parameter.
#

# the OSKit install directory
oskit=/z/oskit

# The gcc version to use.
gcc=gcc

# The bootloader-specific .o to link in
bootloaderObject=multiboot.o
#bootloaderObject=dos.o


# remove all the -lm's from the library list.  We need to link against
# the proper  oskit_<OS>_m library
args="";
is_unix="";

for a in $*; do
    case $a in
    --oskit=*)
	oskit=`echo "$1" | sed 's/[-_a-zA-Z0-9]*=//'`;
	;;
    -v) 
	## XXX A hack to make configure through this script work.
	exec `$gcc --print-prog-name=ld` -v;
	;;
    -lm) 
	;;
    -DOSKIT_UNIX) 
	is_unix=true;
	;;
    *) 
	args="$args $a";
	;;
    esac
done

if test -z "$is_unix"; then
    ### Regular, bare-hardware OSKit link line (see why we hid it?)
    $gcc -v -static -nostartfiles -Wl,-Ttext -Wl,100000	\
	${oskit}/lib/oskit/${bootloaderObject}		\
	$args						\
	-nostdlib 					\
	-L${oskit}/lib					\
	-loskit_startup  				\
	-loskit_clientos				\
	-loskit_threads  				\
	-loskit_svm 					\
	-loskit_amm					\
	-loskit_bootp					\
	-loskit_freebsd_net				\
	-loskit_linux_dev 				\
	-loskit_dev					\
	-loskit_netbsd_fs				\
	-loskit_kern					\
	-loskit_lmm					\
	-loskit_diskpart 				\
	-loskit_memfs					\
	-loskit_freebsd_c_r  				\
	-loskit_fsnamespace_r				\
	-loskit_com					\
	-loskit_freebsd_m 				\
	-loskit_freebsd_c_r				\
	-loskit_threads					\
	${oskit}/lib/oskit/crtn.o 
else
    ### OSKit/UNIX link line (shield our eyes)
    $gcc -v -e _start				\
	${oskit}/lib/unix/crt1.o 		\
	${oskit}/lib/unix/crti.o		\
  	${oskit}/lib/unix/crtbegin.o		\
  	$args					\
  	-nostdlib  				\
	-L${oskit}/lib				\
        ${oskit}/lib/unix_support_pthreads.o	\
        -loskit_startup  			\
	-loskit_clientos 			\
	-loskit_fsnamespace_r			\
        -loskit_threads 			\
	-loskit_unix 				\
	-loskit_dev				\
  	-loskit_freebsd_c_r  			\
	-loskit_com				\
  	-loskit_freebsd_m 			\
	-loskit_freebsd_c_r			\
        -lfreebsdsys				\
        -loskit_threads				\
  	${oskit}/lib/unix/crtend.o 		\
	${oskit}/lib/unix/crtn.o
fi

#eof
