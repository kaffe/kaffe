dnl Checks if function/macro va_copy() is available
dnl Defines HAVE_VA_COPY on success.
AC_DEFUN([AC_FUNC_VA_COPY],
[AC_CACHE_CHECK([for va_copy], ac_cv_func_va_copy,
                                [AC_LINK_IFELSE([AC_LANG_PROGRAM([[
#ifdef  HAVE_STDARG_H
#   include <stdarg.h>
#else
#   include <varargs.h>
#endif]], [[
va_list a, b;

va_copy(a, b);]])],[ac_cv_func_va_copy=yes],[ac_cv_func_va_copy=no])])
if test $ac_cv_func_va_copy = yes; then
  AC_DEFINE(HAVE_VA_COPY, 1, [Define if va_copy is available])
fi])

dnl Checks if function/macro __va_copy() is available
dnl Defines HAVE__VA_COPY on success.
AC_DEFUN([AC_FUNC__VA_COPY],
[AC_CACHE_CHECK([for __va_copy], ac_cv_func__va_copy,
                                [AC_LINK_IFELSE([AC_LANG_PROGRAM([[
#ifdef  HAVE_STDARG_H
#   include <stdarg.h>
#else
#   include <varargs.h>
#endif]], [[
va_list a, b;

__va_copy(a, b);]])],[ac_cv_func__va_copy=yes],[ac_cv_func__va_copy=no])])
if test $ac_cv_func__va_copy = yes; then
  AC_DEFINE(HAVE__VA_COPY, 1, [Define if __va_copy is available])
fi])

dnl Checks if va_list is an array
dnl Defines VA_LIST_IS_ARRAY on success.
AC_DEFUN([AC_TYPE_VA_LIST],
[AC_CACHE_CHECK([if va_list is an array], ac_cv_type_va_list_array,
                                [AC_LINK_IFELSE([AC_LANG_PROGRAM([[
#ifdef  HAVE_STDARG_H
#   include <stdarg.h>
#else
#   include <varargs.h>
#endif
]], [[
va_list a, b;

a = b;]])],[ac_cv_type_va_list_array=no],[ac_cv_type_va_list_array=yes])]) 
if test $ac_cv_type_va_list_array = yes; then
        AC_DEFINE(VA_LIST_IS_ARRAY, 1, [Define if va_list is an array])
fi])
