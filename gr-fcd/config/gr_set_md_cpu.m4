dnl
dnl Copyright 2003,2008,2009 Free Software Foundation, Inc.
dnl 
dnl This file is part of GNU Radio
dnl 
dnl GNU Radio is free software; you can redistribute it and/or modify
dnl it under the terms of the GNU General Public License as published by
dnl the Free Software Foundation; either version 3, or (at your option)
dnl any later version.
dnl 
dnl GNU Radio is distributed in the hope that it will be useful,
dnl but WITHOUT ANY WARRANTY; without even the implied warranty of
dnl MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
dnl GNU General Public License for more details.
dnl 
dnl You should have received a copy of the GNU General Public License
dnl along with GNU Radio; see the file COPYING.  If not, write to
dnl the Free Software Foundation, Inc., 51 Franklin Street,
dnl Boston, MA 02110-1301, USA.
dnl 

AC_DEFUN([_TRY_ADD_ALTIVEC],
[
  LF_CHECK_CC_FLAG([-mabi=altivec -maltivec])
  LF_CHECK_CXX_FLAG([-mabi=altivec -maltivec])
])

AC_DEFUN([_FIND_ARM_ISA],
[
  AC_COMPILE_IFELSE([AC_LANG_PROGRAM(
  [[#ifndef __ARM_ARCH_5__
    #error "Not armv5"
    #endif
  ]])],
    [is_armv5=yes],
    [is_armv5=no])

  AC_COMPILE_IFELSE([AC_LANG_PROGRAM(
  [[#ifndef __ARM_ARCH_7A__
    #error "Not armv7-a"
    #endif
  ]])],
    [is_armv7_a=yes],
    [is_armv7_a=no])

])

AC_DEFUN([GR_SET_MD_CPU],[
  AC_REQUIRE([AC_CANONICAL_HOST])
  AC_ARG_WITH(md-cpu,
	AC_HELP_STRING([--with-md-cpu=ARCH],[set machine dependent speedups (auto)]),
		[cf_with_md_cpu="$withval"],
		[
  dnl see if the user has specified --host or --build, via 'cross_compiling'
  if test "$cross_compiling" != no; then
    dnl when cross-compiling, because the user specified it either via
    dnl --target or --build, just keep the user's specs & hope for the best.
    cf_with_md_cpu="$host_cpu"
  else
    dnl when the user didn't specify --target or --build, on Darwin 10
    dnl (OSX 10.6.0 and .1) and GNU libtoool 2.2.6, 'configure' doesn't
    dnl figure out the CPU type correctly, so do it by hand here using
    dnl the sizeof (void*): if 4 then use i386, and otherwise use x86_64.
    case "$host_os" in
     *darwin*10*)
      AC_CHECK_SIZEOF(void*)
      if test "$ac_cv_sizeof_voidp" = 4; then
       cf_with_md_cpu="i386"
      else
       cf_with_md_cpu="x86_64"
      fi
      ;;
     *)
      cf_with_md_cpu="$host_cpu"
      ;;
    esac
  fi
  ])
  case "$cf_with_md_cpu" in
   x86 | i[[3-7]]86)	MD_CPU=x86	MD_SUBCPU=x86 ;;
   x86_64)		MD_CPU=x86	MD_SUBCPU=x86_64 ;;
   powerpc*)            MD_CPU=powerpc ;;
   arm*)
       _FIND_ARM_ISA
       if test $is_armv5 = yes; then MD_CPU=armv5;
       elif test $is_armv7_a = yes; then MD_CPU=armv7_a;
       else MD_CPU=generic; fi
       ;;
   *)           	MD_CPU=generic ;;
  esac

  AC_ARG_ENABLE(altivec,
    AC_HELP_STRING([--enable-altivec],[enable altivec on PowerPC (yes)]),
    [ if test $MD_CPU = powerpc; then
        case "$enableval" in
          (no)  MD_CPU=generic ;;
          (yes) _TRY_ADD_ALTIVEC ;;
          (*) AC_MSG_ERROR([Invalid argument ($enableval) to --enable-altivec]) ;;
        esac
      fi],
    [ if test $MD_CPU = powerpc; then _TRY_ADD_ALTIVEC fi])


  AC_MSG_CHECKING([for machine dependent speedups])
  AC_MSG_RESULT($MD_CPU)
  AC_SUBST(MD_CPU)
  AC_SUBST(MD_SUBCPU) 

  AM_CONDITIONAL(MD_CPU_x86,     test "$MD_CPU" = "x86")
  AM_CONDITIONAL(MD_SUBCPU_x86_64,  test "$MD_SUBCPU" = "x86_64")
  AM_CONDITIONAL(MD_CPU_powerpc, test "$MD_CPU" = "powerpc")
  AM_CONDITIONAL(MD_CPU_armv5, test "$MD_CPU" = "armv5")
  AM_CONDITIONAL(MD_CPU_armv7_a, test "$MD_CPU" = "armv7_a")
  AM_CONDITIONAL(MD_CPU_generic, test "$MD_CPU" = "generic")
])
