#!/bin/sh
#
# wrapper to setup cross-compilation of firmware
#

for v in CC CXX AR NM RANLIB STRIP F77 CPPFLAGS LDFLAGS 
do
  unset $v
done

`dirname $0`/configure "$@" --host=mb
