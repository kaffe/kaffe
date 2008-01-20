#! /bin/sh

autoreconf -f -i # -Wall

# drops specific patches
(
  cd scripts
  patch -p0 <../developers/config1.patch
)
