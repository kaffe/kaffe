#
# PowerPC/Linux configuration
#
Khost_cpu=powerpc
Khost_os=linux
CFLAGS="-g -O2 -fsigned-char"

# PowerPC does not need libffi for sysdepCallMethod
with_libffi=no
