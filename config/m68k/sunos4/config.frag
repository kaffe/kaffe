#
# m68k/Sunos4 configuration.
#
host_cpu=m68k
host_os=sunos4
PIC=-fPIC
LIBSHARE=-Bdynamic
LIBEXT=.so
if [ "$cross_compiling" = yes ]; then
# if we use cross environment, set tools properly
  ac_cv_prog_AR=${ac_cv_prog_AR='m68k-sun-sunos4.1.1-ar -r'}
# CC should be set before invoking configure, rather than here.
# ac_cv_prog_CC=${ac_cv_prog_CC='m68k-sun-sunos4.1.1-gcc'}
  ac_cv_prog_CPP=${ac_cv_prog_CPP='m68k-sun-sunos4.1.1-gcc -E'}
  ac_cv_prog_RANLIB=${ac_cv_prog_RANLIB='m68k-sun-sunos4.1.1-ranlib'}
# if we use cross environment, following values may not be detected.
  ac_cv_alignmentof_voidp=${ac_cv_alignmentof_voidp='2'}
  ac_cv_c_bigendian=${ac_cv_c_bigendian='yes'}
  ac_cv_sizeof___int64=${ac_cv_sizeof___int64='0'}
  ac_cv_sizeof_int=${ac_cv_sizeof_int='4'}
  ac_cv_sizeof_long=${ac_cv_sizeof_long='4'}
  ac_cv_sizeof_long_long=${ac_cv_sizeof_long_long='8'}
  ac_cv_sizeof_short=${ac_cv_sizeof_short='2'}
  ac_cv_sizeof_voidp=${ac_cv_sizeof_voidp='4'}
fi
