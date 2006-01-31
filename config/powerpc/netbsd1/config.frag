#
# PowerPC/NetBSD configuration
#
CFLAGS="$CFLAGS -fsigned-char"

if [ "$cross_compiling" = yes ]; then
  ac_cv_c_char_unsigned=${ac_cv_c_char_unsigned='no'}
fi

if test "$with_setjmp" = "glibc"; then
    # Use setjmp()/longjmp() from glibc-2.2.2
    CPPFLAGS="$CPPFLAGS -DJTHREAD_JBLEN=58"
elif test "$with_setjmp" = "sigsetjmp"; then
    # Use sigsetjmp()/siglongjmp()
    CPPFLAGS="$CPPFLAGS -DJTHREAD_USE_SIGSETJMP"
fi
