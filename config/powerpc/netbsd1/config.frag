#
# PowerPC/NetBSD configuration
#
Khost_cpu=powerpc
Khost_os=netbsd1

# Use setjmp()/longjmp() from glibc-2.2.2
#CPPFLAGS="$CPPFLAGS -DJTHREAD_JBLEN=58"

# Use sigsetjmp()/siglongjmp()
#CPPFLAGS="$CPPFLAGS -DJTHREAD_USE_SIGSETJMP"

CFLAGS="$CFLAGS -fsigned-char"
