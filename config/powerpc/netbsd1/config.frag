#
# PowerPC/NetBSD configuration
#
Khost_cpu=powerpc
Khost_os=netbsd1

if test "$with_setjmp" = "glibc"; then
    # Use setjmp()/longjmp() from glibc-2.2.2
    CPPFLAGS="$CPPFLAGS -DJTHREAD_JBLEN=58"
elif test "$with_setjmp" = "sigsetjmp"; then
    # Use sigsetjmp()/siglongjmp()
    CPPFLAGS="$CPPFLAGS -DJTHREAD_USE_SIGSETJMP"
fi

CFLAGS="$CFLAGS -fsigned-char"
