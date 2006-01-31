#
# Arm/Linux configuration 
#     
#
# in ARM gcc characters are by default unsigned
#
CFLAGS="$CFLAGS -fsigned-char -fno-omit-frame-pointer"

# if we use cross environment, following values may not be detected.
if [ "$cross_compiling" = yes ]; then
#  ac_cv_func_memcmp_working=${ac_cv_func_memcmp_working='yes'}
  ac_cv_func_mmap_fixed_mapped=${ac_cv_func_mmap_fixed_mapped='yes'}
#  ac_cv_func_utime_null=${ac_cv_func_utime_null='yes'}
  ac_cv_c_char_unsigned=${ac_cv_c_char_unsigned='no'}
fi
