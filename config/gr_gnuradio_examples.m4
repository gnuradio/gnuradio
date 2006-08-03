# Copyright 2001,2002,2003,2004,2005,2006 Free Software Foundation, Inc.
# 
# This file is part of GNU Radio
# 
# GNU Radio is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2, or (at your option)
# any later version.
# 
# GNU Radio is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with GNU Radio; see the file COPYING.  If not, write to
# the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
# Boston, MA 02111-1307, USA.

AC_DEFUN([GR_GNURADIO_EXAMPLES],[
    AC_CONFIG_SRCDIR([gnuradio-examples/README])

    AC_CONFIG_FILES([\
	  gnuradio-examples/Makefile \
	  gnuradio-examples/python/Makefile \
	  gnuradio-examples/python/audio/Makefile \
	  gnuradio-examples/python/digital_voice/Makefile \
	  gnuradio-examples/python/gmsk2/Makefile \
	  gnuradio-examples/python/mc4020/Makefile \
	  gnuradio-examples/python/usrp/Makefile \
	  gnuradio-examples/python/multi_usrp/Makefile \
	])

    subdirs="$subdirs gnuradio-examples"
])
