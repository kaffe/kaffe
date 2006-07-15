#
# x86-64/Linux configuration
#
if test x"$with_threads" = x"unix-pthreads" ; then
	CPPFLAGS="$CPPFLAGS -D_REENTRANT"
fi

# Make sure that the compiler knows about the additional registers
KAFFE_CFLAGS="$KAFFE_CFLAGS -m64"

# if we use cross environment, following values may not be detected.
if [ "$cross_compiling" = yes ]; then
  ac_cv_c_char_unsigned=${ac_cv_c_char_unsigned='no'}
fi
