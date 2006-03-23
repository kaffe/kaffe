#
# Arm/Linux configuration 
#     
#
# in ARM gcc characters are by default unsigned
#
CFLAGS="$CFLAGS -fsigned-char -fno-omit-frame-pointer -D__UCLIBC__ -D__unix__ -DKAFFEMD_BUGGY_STACK_OVERFLOW -DKAFFE_VMDEBUG -D_SCALB_INT -Wl,-elf2flt "

# if we use cross environment, following values may not be detected.
if [ "$cross_compiling" = yes ]; then
#  ac_cv_func_memcmp_working=${ac_cv_func_memcmp_working='yes'}
  ac_cv_func_mmap_fixed_mapped=${ac_cv_func_mmap_fixed_mapped='yes'}
#  ac_cv_func_utime_null=${ac_cv_func_utime_null='yes'}
  ac_cv_c_char_unsigned=${ac_cv_c_char_unsigned='no'}
fi
