#!/bin/sh

# the oskit install directory
oskit=/z/oskit

# remove all the -lm's from the library list.  We need to link against
# the proper  oskit_<OS>_m library
args="";
is_unix="";

for a in $*
do
    case $a in
    -lm) ;;
    -DOSKIT_UNIX) is_unix=true;;
    *) args="$args $a";;
    esac
done

echo "supplied args $*"
echo "transformed to $args"

if [ -z "$is_unix" ] 
then

gcc -v -static -nostartfiles -Ttext 100000	\
   ${oskit}/lib/multiboot.o  \
   $args					\
   -nostdlib -L${oskit}/lib			\
   -loskit_startup  -loskit_clientos		\
   -loskit_threads  -loskit_svm -loskit_amm	\
   -loskit_bootp				\
   -loskit_freebsd_net				\
   -loskit_linux_dev -loskit_dev		\
   -loskit_netbsd_fs				\
   -loskit_kern					\
   -loskit_lmm					\
   -loskit_diskpart -loskit_memfs		\
   -loskit_freebsd_c_r  -loskit_fsnamespace_r \
   -loskit_com		\
   -loskit_freebsd_m -loskit_freebsd_c_r	\
   -loskit_threads	\
   ${oskit}/lib/crtn.o 
else

gcc -v -e _start						\
	${oskit}/lib/unix/crt1.o ${oskit}/lib/unix/crti.o	\
  	${oskit}/lib/unix/crtbegin.o				\
  	$args							\
  	-nostdlib  -L${oskit}/lib				\
        ${oskit}/lib/unix_support_pthreads.o			\
        -loskit_startup  -loskit_clientos -loskit_fsnamespace_r	\
        -loskit_threads -loskit_unix -loskit_dev		\
  	-loskit_freebsd_c_r  -loskit_com			\
  	-loskit_freebsd_m -loskit_freebsd_c_r			\
        ${oskit}/examples/unix/freebsd/libfreebsdsys.a		\
        -loskit_threads						\
  	${oskit}/lib/unix/crtend.o ${oskit}/lib/unix/crtn.o

fi