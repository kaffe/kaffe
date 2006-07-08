#
# Alpha/OSF configuration
#
Khost_cpu=alpha
Khost_os=osf
KAFFE_CFLAGS="$KAFFE_CFLAGS -mieee"

# to use libexc, uncomment theses two lines
if test x"$with_libexc" = x"yes" ; then
	KAFFE_CFLAGS="$KAFFE_CFLAGS -DUSE_LIBEXC"
	LIBS="$LIBS -lexc"
fi

# configure don't detect that -B flag work
NM="${NM:-/usr/bin/nm -B}"
