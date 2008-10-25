#!/bin/sh
#
# wrapper to setup cross-compilation of firmware
#

for v in CC CPP CXX AR NM RANLIB STRIP F77 CFLAGS CXXFLAGS CPPFLAGS LDFLAGS 
do
  unset $v
done

args=
for t in "$@"
do
  case "$t" in
      (CC=*)       ;;
      (CPP=*)       ;;
      (CXX=*)      ;;
      (AR=*)       ;;
      (NM=*)       ;;
      (RANLIB=*)   ;;
      (STRIP=*)    ;;
      (F77=*)      ;;
      (CFLAGS=*)   ;;
      (CXXFLAGS=*) ;;
      (CPPFLAGS=*) ;;
      (LDFLAGS=*)  ;;
      (*)  args="$args $t" ;;
  esac
done


`dirname $0`/configure CFLAGS='-O2' $args --host=mb
