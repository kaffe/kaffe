dnl @synopsis AC_C_COMPILE_VALUE (COMPILE-VALUE, ALIAS, INCLUDES)
dnl
dnl The AC_C_COMPILE_VALUE macro determines a compile time value
dnl by generating the object code and reading the value from the
dnl code.  Static data initializers like sizeof(int) are
dnl unavailable to preprocessor.  The macro calculates the values
dnl known to compiler's static initializer.
dnl
dnl Assumptions:
dnl The sought value should not exceed 65535.
dnl The shell interpreter and the sed utility are expected to
dnl exist and work similarly across possible build platforms.
dnl
dnl Result:
dnl The resulting configure script will generate the preprocessor
dnl symbol definition:
dnl
dnl   #define COMPILE_VALUE_<ALIAS> <NUMBER>
dnl
dnl It was important that the value was embedded into the object
dnl file in a predefined byte order during the test.  This ensured
dnl that the result was independent from the target platform's byte
dnl order.
dnl
dnl The existing AC_CHECK_SIZEOF macro also computes the size of
dnl the given type without running the test program.   However, the
dnl existing macro will produce a piece of configure script that
dnl will take the time proportional to the logarithm of the sought
dnl value.
dnl
dnl Example of use in configure.in:
dnl
dnl   AC_C_COMPILE_VALUE(sizeof(int), sizeof_int)
dnl   AC_C_COMPILE_VALUE([sizeof(int[[543]])], sizeof_int543)
dnl
dnl As a result of runnfing the generated configure script, the
dnl following definition will appear in config.h:
dnl
dnl   #define COMPILE_VALUE_SIZEOF_INT 4
dnl   #define COMPILE_VALUE_SIZEOF_INT543 2172
dnl
dnl @version $Id: compile_value.m4,v 1.1 2004/07/03 21:16:24 dalibor Exp $
dnl @author Ilguiz Latypov <ilatypov@superbt.com>
dnl
## Portability defines that help interoperate with classic and modern autoconfs
ifdef([AC_TR_SH],[
define([AC_TR_SH_REUSE],[AC_TR_SH([$1])])
define([AC_TR_CPP_REUSE],[AC_TR_CPP([$1])])
], [
define([AC_TR_SH_REUSE],
       [patsubst(translit([[$1]], [*+], [pp]), [[^a-zA-Z0-9_]], [_])])
define([AC_TR_CPP_REUSE],
       [patsubst(translit([[$1]],
                          [*abcdefghijklmnopqrstuvwxyz],
                          [PABCDEFGHIJKLMNOPQRSTUVWXYZ]),
                 [[^A-Z0-9_]], [_])])
])

AC_DEFUN([AC_C_COMPILE_VALUE], [
  pushdef([ac_c_compile_value],
    AC_TR_SH_REUSE([ac_cv_c_compile_value_$2]))dnl
  ac_c_compile_value_expand="$1"
  AC_CACHE_CHECK([value of $1 by analyzing object code],
                 ac_c_compile_value, [
    save_CFLAGS="$CFLAGS"
    CFLAGS="$CFLAGS -c -o conftest.o"
    AC_TRY_COMPILE([$3
      #include <stddef.h>
      #include <stdint.h>
      #include <stdlib.h>
      #define COMPILE_VALUE $ac_c_compile_value_expand
      #define HEX_DIGIT(n)      ((n) >= 10 ? 'a' + (n) - 10 : '0' + (n))
      char object_code_block[] = {
        '\n', 'e', '4', 'V', 'A',
        '0', 'x',
        (char) HEX_DIGIT((((COMPILE_VALUE / 16) / 16) / 16) % 16),
        (char) HEX_DIGIT(((COMPILE_VALUE / 16) / 16) % 16),
        (char) HEX_DIGIT((COMPILE_VALUE / 16) % 16),
        (char) HEX_DIGIT(COMPILE_VALUE % 16),
        'Y', '3', 'p', 'M', '\n'
      };],
      [],
      [ac_c_compile_value=`
        typeset -i n=\`sed -ne 's/^e4VA0x\(.*\)Y3pM$/0x\1/p' < conftest.o\`;
        echo $n`],
      [ac_c_compile_value=0])
    CFLAGS="$save_CFLAGS"])
  AC_DEFINE_UNQUOTED(AC_TR_CPP_REUSE(compile_value_$2),
                     [$[]ac_c_compile_value],
                     [$1])
  popdef([ac_c_compile_value])dnl
])
