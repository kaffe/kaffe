#
# Arm/Linux configuration 
#     
host_cpu=arm
host_os=linux
 
#
# Default engine is the interpreter, until the jit is proved to work
#
#with_engine=intrp

#
# Enable use of shared libraries
#
PIC=-fPIC
LIBSHARE=-shared
LIBEXT=.so

# Uncomment the following two lines to make the VM part of
# the invocation program (kaffe).  This is necessary for debugging
# on some systems (notable Redhat 5.0).
#KAFFELINK="-Wl,--export-dynamic"
#vm_dynamic_library=no

#
# in ARM gcc characters are by default unsigned
#
CFLAGS="$CFLAGS -fsigned-char"
