#
# Copyright 2011 Free Software Foundation, Inc.
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

AM_CPPFLAGS = $(STD_DEFINES_AND_INCLUDES) $(PYTHON_CPPFLAGS) $(WITH_INCLUDES)

# These headers get installed in ${prefix}/include/gnuradio
grinclude_HEADERS = 			\
	digital_constellation.h		\
	digital_costas_loop_cc.h	\
	digital_cma_equalizer_cc.h	\
	digital_kurtotic_equalizer_cc.h

lib_LTLIBRARIES = libgnuradio-digital.la

libgnuradio_digital_la_SOURCES = 	\
	digital_constellation.cc	\
	digital_costas_loop_cc.cc	\
	digital_cma_equalizer_cc.cc	\
	digital_kurtotic_equalizer_cc.cc

libgnuradio_digital_la_LIBADD =	\
	$(GNURADIO_CORE_LA)

libgnuradio_digital_la_LDFLAGS = $(NO_UNDEFINED) $(LTVERSIONFLAGS)
