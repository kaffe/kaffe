#
# Arm/Netbsd1 configuration 
#     
vm_dynamic_library=no

if [ "$cross_compiling" = yes ]; then
  ac_cv_c_char_unsigned=${ac_cv_c_char_unsigned='no'}
fi
