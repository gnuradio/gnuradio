dnl Copyright 2001,2002,2003,2004,2005,2006,2008 Free Software Foundation, Inc.
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

AC_DEFUN([GRC_GCELL],[
    GRC_ENABLE(gcell)
    dnl GRC_WITH(gcell)

    dnl Don't do gcell if omnithread skipped
    GRC_CHECK_DEPENDENCY(gcell, omnithread)

    dnl If execution gets to here, $passed will be:
    dnl   with : if the --with code didn't error out
    dnl   yes  : if the --enable code passed muster and all dependencies are met
    dnl   no   : otherwise
    if test $passed = yes; then
	dnl Don't build gcell if host_cpu isn't powerpc
	AC_MSG_CHECKING([whether host_cpu is powerpc*])
	case "$host_cpu" in
	    powerpc*)
		AC_MSG_RESULT(yes)
	        ;;
            *)
	        passed=no
		AC_MSG_RESULT(no)
		;;
	esac

        dnl Don't build gcell if we can't find spu-gcc.
	AC_CHECK_PROG([SPU_GCC_PROG],[spu-gcc],[yes],[no])
	if test $SPU_GCC_PROG = no; then
            passed=no
        fi
    fi

    if test $passed != with; then
	dnl how and where to find INCLUDES and LA
	gcell_INCLUDES="-I\${abs_top_srcdir}/gcell/src/include \
		 -I\${abs_top_srcdir}/gcell/src/lib/runtime \
		 -I\${abs_top_srcdir}/gcell/src/lib/general \
		 -I\${abs_top_srcdir}/gcell/src/lib/wrapper"
        gcell_LA="\${abs_top_builddir}/gcell/src/lib/libgcell.la"
	gcell_spu_INCLUDES="-I\${abs_top_srcdir}/gcell/src/include/spu \
		 -I\${abs_top_srcdir}/gcell/src/include \
		 -I\${abs_top_srcdir}/gcell/src/lib/runtime/spu \
		 -I\${abs_top_srcdir}/gcell/src/lib/general/spu \
		 -I\${abs_top_srcdir}/gcell/src/lib/wrapper/spu"
	gcell_spu_LA="\${abs_top_builddir}/gcell/src/lib/spu/libgcell_spu.a"
	AC_SUBST(gcell_spu_INCLUDES)
	AC_SUBST(gcell_spu_LA)

        dnl kludge up initial swig dependency files
        AC_CONFIG_COMMANDS([swig_gcell_deps], [
            touch gr-gcell/src/gcell.d
        ])
    fi

    AC_CONFIG_FILES([ \
        gcell/Makefile \
        gcell/gcell.pc \
        gcell/src/Makefile \
        gcell/src/include/Makefile \
        gcell/src/include/spu/Makefile \
        gcell/src/lib/Makefile \
        gcell/src/lib/spu/Makefile \
        gcell/src/lib/general/Makefile \
        gcell/src/lib/wrapper/Makefile \
        gcell/src/lib/runtime/Makefile \
        gcell/src/apps/Makefile \
	gcell/src/apps/spu/Makefile \
	gcell/src/ibm/Makefile \
    ])

    GRC_BUILD_CONDITIONAL(gcell)
])
