#
# sh/Linux configuration.
#
# Written by Kiyo Inaba <k-inaba@mxb.mesh.ne.jp>, 2002;
#	to support cross compilation and endian.
#
Khost_cpu=sh
Khost_os=linux
CFLAGS="-g"
if [ "$cross_compiling" = yes ]; then
# In cross environment, following values may not be detected automatically.
  ac_cv_alignmentof_voidp=${ac_cv_alignmentof_voidp='4'}
  ac_cv_sizeof___int64=${ac_cv_sizeof___int64='0'}
  ac_cv_sizeof_int=${ac_cv_sizeof_int='4'}
  ac_cv_sizeof_long=${ac_cv_sizeof_long='4'}
  ac_cv_sizeof_long_long=${ac_cv_sizeof_long_long='8'}
  ac_cv_sizeof_short=${ac_cv_sizeof_short='2'}
  ac_cv_sizeof_voidp=${ac_cv_sizeof_voidp='4'}
# Endian can be set by check the name of compiler, ugly?
  if [ "$CC" = 'sh3eb-linux-gcc' ]; then
    ac_cv_c_bigendian=${ac_cv_c_bigendian='yes'}
  else
    ac_cv_c_bigendian=${ac_cv_c_bigendian='no'}
  fi
fi
