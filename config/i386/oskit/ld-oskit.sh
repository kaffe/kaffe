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

# the OSKit install directory
oskit=/z/oskit

# The gcc version to use.
gcc=gcc

# The bootloader-specific .o to link in
bootloaderObject=multiboot.o
#bootloaderObject=dos.o

# Uncomment/Comment this line to include/remove memdebug support
loskit_memdebug="-loskit_memdebug"

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
    --oskitunix) 
	is_unix=true;
	;;
    kaffeh)
	### XXX This is a bad hack to prevent trying to link kaffeh
	rm -f kaffeh
	echo "#!/bin/sh" > kaffeh
	echo "echo KAFFEH HACK HAS STRUCK.  See $0" >> kaffeh
	echo "#eof" > kaffeh
	echo "KAFFEH NOT REALLY BUILT.  THIS IS A HACK BECAUSE IT WILL NOT LINK AGAINST THE OSKIT."
	exit 0
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
	-Wl,--start-group				\
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
	-loskit_lmm					\
	-loskit_diskpart 				\
	-loskit_memfs					\
	-loskit_fsnamespace_r				\
	-loskit_com					\
	-loskit_kern					\
	-loskit_freebsd_m 				\
	$loskit_memdebug 				\
	-loskit_freebsd_c_r				\
	-Wl,--end-group					\
	${oskit}/lib/oskit/crtn.o 
else
    ### OSKit/UNIX link line (shield your eyes)
    $gcc -v -e _start				\
	${oskit}/lib/oskit/unix/crt1.o 		\
	${oskit}/lib/oskit/unix/crti.o		\
        ${oskit}/lib/oskit/unix_support_pthreads.o	\
  	$args					\
  	-nostdlib  				\
	-L${oskit}/lib				\
        -loskit_startup  			\
	-loskit_clientos 			\
        -loskit_threads 			\
	-loskit_fsnamespace_r			\
	$loskit_memdebug 			\
	-loskit_unix 				\
	-loskit_freebsd_net 			\
	-loskit_linux_dev 			\
	-loskit_dev				\
  	-loskit_freebsd_c_r  			\
	-loskit_lmm 				\
	-loskit_com				\
  	-loskit_freebsd_m 			\
	-loskit_freebsd_c_r			\
        -loskit_threads				\
	${oskit}/lib/oskit/unix/crtn.o
fi

#eof
