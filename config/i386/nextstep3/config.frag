#
# i386/Nextstep3 configuration
#
Khost_cpu=i386
Khost_os=nextstep3
CFLAGS="-g -seglinkedit -all_load"
LIBSHARE=-r
LIBEXT=.o
vm_dynamic_library=no
