#
# m68k/Netbsd1 configuration.
#
CFLAGS="$CFLAGS -fno-omit-frame-pointer"
LDFLAGS="$LDFLAGS -lm68k"
if [ "$cross_compiling" = yes ]; then
  ac_cv_c_char_unsigned=${ac_cv_c_char_unsigned='no'}
fi
