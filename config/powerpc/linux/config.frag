#
# PowerPC/Linux configuration
#
Khost_cpu=powerpc
Khost_os=linux
CFLAGS="-g -fsigned-char"

# PowerPC needs libffi for sysdepCallMethod
with_libffi=yes