#
# Darwin PowerPC (Mac OS X) configuration
#
Khost_cpu=powerpc
Khost_os=darwin

## Darwin 5 needs -no-cpp-precomp to turn the 
## use of precompiled headers off, which leads
## to wrong warnings and build failures.

CFLAGS="$CFLAGS -fsigned-char -no-cpp-precomp"
