#
# Mips/Netbsd configuration.
#
KAFFE_CFLAGS="$KAFFE_CFLAGS -fno-omit-frame-pointer"

if [ "$cross_compiling" = yes ]; then
  ac_cv_c_char_unsigned=${ac_cv_c_char_unsigned='no'}
fi
