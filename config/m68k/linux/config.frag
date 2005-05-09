#
# m68k/Linux configuration.
#
CFLAGS="$CFLAGS -O1 -fno-omit-frame-pointer"

# if we use cross environment, following values may not be detected.
if [ "$cross_compiling" = yes ]; then
  ac_cv_alignmentof_voidp=${ac_cv_alignmentof_voidp='2'}
  ac_cv_c_char_unsigned=${ac_cv_c_char_unsigned='no'}
fi
