#
# i386/GNU Hurd configuration
#
Khost_cpu=i386
Khost_os=gnu

# unix-jthread quickfix
CPPFLAGS="$CPPFLAGS -D_HURD_ASYNC_QUICKFIX_"
