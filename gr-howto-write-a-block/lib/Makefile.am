#
# Copyright 2004,2005,2006,2008,2009 Free Software Foundation, Inc.
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
# 

include $(top_srcdir)/Makefile.common

###################################
# howto C++ library

# C/C++ headers get installed in ${prefix}/include/gnuradio
grinclude_HEADERS =		\
	howto_square_ff.h	\
	howto_square2_ff.h

lib_LTLIBRARIES = libgnuradio-howto.la

libgnuradio_howto_la_SOURCES = 	\
	howto_square_ff.cc	\
	howto_square2_ff.cc

libgnuradio_howto_la_LIBADD =	\
	$(GNURADIO_CORE_LA)

libgnuradio_howto_la_LDFLAGS =	\
	$(NO_UNDEFINED)
