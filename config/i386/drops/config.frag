echo "**********************************************************************"
echo "* Use config.frag of DROPS                                           *"
echo "**********************************************************************"

cross_compiling='yes'

ac_cv_c_char_unsigned='no'   # char is signed
# dietlibc issue/problems
ac_cv_func_getaddrinfo='yes'
ac_cv_func_getnameinfo='yes'
ac_cv_header_time_h='no'  # global variable 'tzname'  not available
ac_cv_var_tzname='no'
ac_cv_func_localtime='no'     # no full support 
ac_cv_func_shl_load='no'      # no support
ac_cv_func_kill='no'          # we have no signals at all
ac_cv_func_MD2Init='no'       # no support
ac_cv_func_MD4Init='no'       # no support
ac_cv_func_MD5Init='no'       # no support 
am_cv_val_LC_MESSAGES='no'    # no support
ac_cv_member_struct_tm_tm_zone='no'
