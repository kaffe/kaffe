#
# Arm/Linux configuration 
#     
#
# in ARM gcc characters are by default unsigned
#
KAFFE_CFLAGS="$KAFFE_CFLAGS -fsigned-char"

# if we use cross environment, following values may not be detected.
if [ "$cross_compiling" = yes ]; then
#  ac_cv_func_memcmp_working=${ac_cv_func_memcmp_working='yes'}
  ac_cv_func_mmap_fixed_mapped=${ac_cv_func_mmap_fixed_mapped='yes'}
#  ac_cv_func_utime_null=${ac_cv_func_utime_null='yes'}
  ac_cv_c_char_unsigned=${ac_cv_c_char_unsigned='no'}
fi
