#
# We do not support shared libraries yet
#
dynamic_libraries=no

#
# Unfortunately, it doesn't pass GCTest when preemption is enabled,
# so old threads only for now.  XXX
#
with_threads=unix-internal
