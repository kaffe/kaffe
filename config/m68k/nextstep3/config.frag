#
# m68k/Nextstep3 configuration.
#
host_cpu=m68k
host_os=nextstep3
CFLAGS="-g -seglinkedit -all_load"
LIBSHARE=-r
LIBEXT=.o
vm_dynamic_library=no
