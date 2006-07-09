#
# m68k/Netbsd1 configuration.
#
KAFFE_CFLAGS="$KAFFE_CFLAGS -O1"
LDFLAGS="$LDFLAGS -lm68k"
if [ "$cross_compiling" = yes ]; then
  ac_cv_c_char_unsigned=${ac_cv_c_char_unsigned='no'}
fi
