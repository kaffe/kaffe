#
# ARM/RISC OS configuration
#     
Khost_cpu=arm
Khost_os=riscos
#
# On ARM compilers characters are by default unsigned
#
CFLAGS="$CFLAGS -fsigned-char -mpoke-function-name"

if [ "$cross_compiling" = yes ]; then
# if we use cross environment, following values may not be detected.
  ac_cv_alignmentof_voidp=${ac_cv_alignmentof_voidp='4'}
  ac_cv_c_bigendian=${ac_cv_c_bigendian='no'}
  ac_cv_sizeof___int64=${ac_cv_sizeof___int64='0'}
  ac_cv_sizeof_int=${ac_cv_sizeof_int='4'}
  ac_cv_sizeof_long=${ac_cv_sizeof_long='4'}
  ac_cv_sizeof_long_long=${ac_cv_sizeof_long_long='8'}
  ac_cv_sizeof_short=${ac_cv_sizeof_short='2'}
  ac_cv_sizeof_voidp=${ac_cv_sizeof_voidp='4'}
  ac_cv_func_mmap_fixed_mapped=${ac_cv_func_mmap_fixed_mapped='yes'}
fi
