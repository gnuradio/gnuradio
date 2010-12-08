#!/bin/bash

cp volk_config.h include/volk/
cd lib
srcdir=../$3

case $1 in
    (x86)
    
    case $2 in
	(x86_64)
	gcc -o volk_mktables -I$srcdir/include -I$srcdir/lib -I../include -I. $srcdir/lib/volk_cpu_x86.c $srcdir/lib/cpuid_x86_64.S $srcdir/lib/volk_rank_archs.c $srcdir/lib/volk_mktables.c
	./volk_mktables
	;;
	(*)
	gcc -o volk_mktables -I$srcdir/include -I$srcdir/lib -I../include -I. $srcdir/lib/volk_cpu_x86.c $srcdir/lib/cpuid_x86.S $srcdir/lib/volk_rank_archs.c $srcdir/lib/volk_mktables.c
	./volk_mktables
	;;
    esac
    ;;
    (powerpc)
    gcc -o volk_mktables -I$srcdir/include -I$srcdir/lib -I../include -I. $srcdir/lib/volk_cpu_powerpc.c $srcdir/lib/volk_rank_archs.c $srcdir/lib/volk_mktables.c
    ./volk_mktables
    ;;
    (*)
    gcc -o volk_mktables -I$srcdir/include -I$srcdir/lib -I../include -I. $srcdir/lib/volk_cpu_generic.c $srcdir/lib/volk_rank_archs.c $srcdir/lib/volk_mktables.c
    ./volk_mktables
    ;;
esac
mv volk_tables.h ../include/volk/

