#
# Arm/Linux configuration 
#     
host_cpu=arm
host_os=linux
#
# in ARM gcc characters are by default unsigned
#
CFLAGS="$CFLAGS -fsigned-char"
