#
# Sparc/Linux configuration
#
# if we use cross environment, following values may not be detected.
if [ "$cross_compiling" = yes ]; then
  ac_cv_c_char_unsigned=${ac_cv_c_char_unsigned='no'}
  ac_cv_file__proc_self_maps=${ac_cv_file__proc_self_maps='yes'}
fi
