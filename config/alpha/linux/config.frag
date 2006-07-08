#
# Alpha/Linux configuration
#
KAFFE_CFLAGS="$KAFFE_CFLAGS -mieee"

# if we use cross environment, following values may not be detected.
if [ "$cross_compiling" = yes ]; then
  ac_cv_c_char_unsigned=${ac_cv_c_char_unsigned='no'}
fi
