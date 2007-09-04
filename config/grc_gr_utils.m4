# Copyright 2007 Free Software Foundation, Inc.
# 
# This file is part of GNU Radio
# 
# GNU Radio is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3, or (at your option)
# any later version.
# 
# GNU Radio is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with GNU Radio; see the file COPYING.  If not, write to
# the Free Software Foundation, Inc., 51 Franklin Street,
# Boston, MA 02110-1301, USA.

AC_DEFUN([GRC_GR_UTILS],[
    GRC_ENABLE([gr-utils])

    AC_CONFIG_FILES([ \
        gr-utils/Makefile \
	gr-utils/src/Makefile \
	gr-utils/src/lib/Makefile \
	gr-utils/src/python/Makefile \
    ])

    passed=yes
    # Don't do gr-utils if gnuradio-core, usrp, or gr-wxgui skipped
    # There *has* to be a better way to check if a value is in a string
    for dir in $skipped_dirs
    do
	if test x$dir = xusrp; then
	    AC_MSG_RESULT([Component gr-utils requires usrp, which is not being built.])
	    passed=no
	fi
	if test x$dir = xgnuradio-core; then
	    AC_MSG_RESULT([Component gr-utils requires gnuradio-core, which is not being built.])
	    passed=no
	fi
	if test x$dir = xgr-wxgui; then
	    AC_MSG_RESULT([Component gr-utils requires gr-wxgui, which is not being built.])
	    passed=no
	fi
    done

    GRC_BUILD_CONDITIONAL([gr-utils])
])
