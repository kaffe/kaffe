#
# i386/SCO3 configuration
#
host_cpu=i386
host_os=sco3.2v5
PIC=-fPIC
LIBSHARE=-G
LIBEXT=.so
LDTAIL="$(LIBS) $LDTAIL"
