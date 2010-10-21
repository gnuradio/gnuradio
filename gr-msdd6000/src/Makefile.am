#
# Copyright 2007,2008,2009 Free Software Foundation, Inc.
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
# the Free Software Foundation, Inc., 51 Franklin Street,
# Boston, MA 02110-1301, USA.
# 

include $(top_srcdir)/Makefile.common

AM_CPPFLAGS = $(STD_DEFINES_AND_INCLUDES) $(PYTHON_CPPFLAGS) \
	$(WITH_INCLUDES)

# C/C++ headers get installed in ${prefix}/include/gnuradio
grinclude_HEADERS =			\
	msdd_source_simple.h		\
	msdd_buffer_copy_behaviors.h	\
	msdd6000.h			\
	msdd_rs_source_simple.h		\
	msdd6000_rs.h

lib_LTLIBRARIES = libgnuradio-msdd6000.la \
	libgnuradio-msdd6000_rs.la

libgnuradio_msdd6000_la_LDFLAGS = $(NO_UNDEFINED) $(LTVERSIONFLAGS)
libgnuradio_msdd6000_rs_la_LDFLAGS = $(NO_UNDEFINED) $(LTVERSIONFLAGS)

libgnuradio_msdd6000_la_SOURCES = 	\
	msdd_source_simple.cc		\
	msdd6000.cc			\
	python/msdd.cc

libgnuradio_msdd6000_rs_la_SOURCES = 	\
	msdd_rs_source_simple.cc	\
	msdd6000_rs.cc			\
	python/msdd_rs.cc

libgnuradio_msdd6000_la_LIBADD = 	\
	$(GNURADIO_CORE_LA)

libgnuradio_msdd6000_rs_la_LIBADD = 	\
	$(GNURADIO_CORE_LA)


if PYTHON
#################################
# SWIG interface and library

TOP_SWIG_IFILES =			\
	msdd.i msdd_rs.i

# Install so that they end up available as:
#   import gnuradio.msdd
# This ends up at:
#   ${prefix}/lib/python${python_version}/site-packages/gnuradio
msdd_pythondir_category =		\
	gnuradio

msdd_rs_pythondir_category =	\
	gnuradio

# additional libraries for linking with the SWIG-generated library
msdd_la_swig_libadd =			\
	libgnuradio-msdd6000.la

msdd_rs_la_swig_libadd =			\
	libgnuradio-msdd6000_rs.la

include $(top_srcdir)/Makefile.swig

# add some of the variables generated inside the Makefile.swig.gen
BUILT_SOURCES = $(swig_built_sources)

# Do not distribute the output of SWIG
no_dist_files = $(swig_built_sources)
endif
