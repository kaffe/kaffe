#
# Alpha/Netbsd1 configuration
#
KAFFE_CFLAGS="$KAFFE_CFLAGS -mieee"

if [ "$cross_compiling" = yes ]; then
  ac_cv_c_char_unsigned=${ac_cv_c_char_unsigned='no'}
fi
