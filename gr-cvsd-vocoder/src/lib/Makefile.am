#
# Copyright 2004,2005,2008,2009 Free Software Foundation, Inc.
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
grinclude_HEADERS =			\
	cvsd_decode_bs.h		\
	cvsd_encode_sb.h

lib_LTLIBRARIES = libgnuradio-cvsd-vocoder.la

libgnuradio_cvsd_vocoder_la_SOURCES = 	\
	cvsd_decode_bs.cc		\
	cvsd_encode_sb.cc

libgnuradio_cvsd_vocoder_la_LIBADD =	\
	$(GNURADIO_CORE_LA)

libgnuradio_cvsd_vocoder_la_LDFLAGS = $(NO_UNDEFINED) $(LTVERSIONFLAGS)

if PYTHON
###################################
# SWIG Python interface and library

TOP_SWIG_IFILES = 			\
	cvsd_vocoder.i

# Install so that they end up available as:
#   import gnuradio.vocoder.cvsd_vocoder
# This ends up at:
#   ${prefix}/lib/python${python_version}/site-packages/gnuradio/vocoder
cvsd_vocoder_pythondir_category =	\
	gnuradio/vocoder

# additional libraries for linking with the SWIG-generated library
cvsd_vocoder_la_swig_libadd =		\
	libgnuradio-cvsd-vocoder.la

include $(top_srcdir)/Makefile.swig

# add some of the variables generated inside the Makefile.swig.gen
BUILT_SOURCES = $(swig_built_sources)

# Do not distribute the output of SWIG
no_dist_files = $(swig_built_sources)
endif
