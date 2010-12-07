#!/bin/bash

cp volk_config.h include/volk/
cd lib
case $1 in
    (x86)
    
    case $2 in
	(x86_64)
	gcc -o volk_mktables -I../include -I. volk_cpu_x86.c cpuid_x86_64.S volk_rank_archs.c volk_mktables.c
	./volk_mktables
	;;
	(*)
	gcc -o volk_mktables -I../include -I. volk_cpu_x86.c cpuid_x86.S volk_rank_archs.c volk_mktables.c
	./volk_mktables
	;;
    esac
    ;;
    (powerpc)
    gcc -o volk_mktables -I../include -I. volk_cpu_powerpc.c volk_rank_archs.c volk_mktables.c
    ./volk_mktables
    ;;
    (*)
    gcc -o volk_mktables -I../include -I. volk_cpu_generic.c volk_rank_archs.c volk_mktables.c
    ./volk_mktables
    ;;
esac
mv volk_tables.h ../include/volk/

