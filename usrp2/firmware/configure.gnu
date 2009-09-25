#!/bin/sh
#
# wrapper to setup cross-compilation of firmware
#

for v in CC CPP CXX AS AR NM RANLIB STRIP F77 CFLAGS CXXFLAGS CPPFLAGS LDFLAGS CCAS CCASFLAGS USB_LIBS USB_CFLAGS
do
  unset $v
done

args=
for t in "$@"
do
  case "$t" in
      (CC=*)       ;;
      (CPP=*)      ;;
      (CXX=*)      ;;
      (AR=*)       ;;
      (AS=*)       ;;
      (NM=*)       ;;
      (RANLIB=*)   ;;
      (STRIP=*)    ;;
      (F77=*)      ;;
      (FFLAGS=*)   ;;
      (CFLAGS=*)   ;;
      (CXXFLAGS=*) ;;
      (CPPFLAGS=*) ;;
      (LDFLAGS=*)  ;;
      (CCAS=*)     ;;
      (CCASFLAGS=*)  ;;
      (USB_CFLAGS=*) ;;
      (USB_LIBS=*) ;;
      (*)  args="$args $t" ;;
  esac
done


`dirname $0`/configure CFLAGS='-O2' $args --host=mb
