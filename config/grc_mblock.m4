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

AC_DEFUN([GRC_MBLOCK],[
    GRC_ENABLE(mblock)

    GRC_WITH(mblock)

    dnl Don't do mblock if omnithread or pmt skipped
    GRC_CHECK_DEPENDENCY(mblock, pmt)
    GRC_CHECK_DEPENDENCY(mblock, omnithread)

    dnl If execution gets to here, $passed will be:
    dnl   with : if the --with code didn't error out
    dnl   yes  : if the --enable code passed muster and all dependencies are met
    dnl   no   : otherwise
    if test $passed = yes; then
	dnl Don't do mblock if guile not available
	GRC_CHECK_GUILE(mblock)
    fi
    if test $passed != with; then
	dnl how and where to find INCLUDES and LA
	mblock_INCLUDES="-I\${abs_top_srcdir}/mblock/src/include"
        mblock_LA="\${abs_top_builddir}/mblock/src/lib/libmblock.la"
    fi

    AC_CONFIG_FILES([\
        mblock/Makefile \
        mblock/mblock.pc \
        mblock/doc/Makefile \
        mblock/src/Makefile \
	mblock/src/include/Makefile \
	mblock/src/include/mblock/Makefile \
        mblock/src/lib/Makefile \
        mblock/src/scheme/Makefile \
        mblock/src/scheme/gnuradio/Makefile \
    ])

    GRC_BUILD_CONDITIONAL(mblock,[
        dnl run_tests is created from run_tests.in.  Make it executable.
	dnl AC_CONFIG_COMMANDS([run_tests_mblock], [chmod +x mblock/src/python/run_tests])
    ])
])
