#
# i386/BeOS configuration
#
host_cpu=i386
host_os=beos
with_threads=beos-native

ac_cv_typedef_int8=yes
ac_cv_typedef_uint8=yes
ac_cv_typedef_int16=yes
ac_cv_typedef_uint16=yes
ac_cv_typedef_int32=yes
ac_cv_typedef_uint32=yes
ac_cv_typedef_int64=yes
ac_cv_typedef_uint64=yes
ac_cv_typedef_bool=yes

CFLAGS="-DDEBUG -g -O2 -Wall -Wstrict-prototypes"
