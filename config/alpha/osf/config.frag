#
# Alpha/OSF configuration
#
Khost_cpu=alpha
Khost_os=osf
CFLAGS="$CFLAGS -mieee"
LIBS="$LIBS -lexc"

# configure don't detect that -B flag work
NM="${NM:-/usr/bin/nm -B}"
