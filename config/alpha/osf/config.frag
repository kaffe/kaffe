#
# Alpha/OSF configuration
#
Khost_cpu=alpha
Khost_os=osf
CFLAGS="$CFLAGS -mieee"

# to use libexc, uncomment theses two lines
if test x"$with_libexc" = x"yes" ; then
	CFLAGS="$CFLAGS -DUSE_LIBEXC"
	LIBS="$LIBS -lexc"
fi

# configure don't detect that -B flag work
NM="${NM:-/usr/bin/nm -B}"
