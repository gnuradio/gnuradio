#
# Copyright 2004,2005,2008,2009,2010 Free Software Foundation, Inc.
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
include $(top_srcdir)/Makefile.swig

SUBDIRS = gsm .

AM_CPPFLAGS = $(STD_DEFINES_AND_INCLUDES) $(PYTHON_CPPFLAGS) $(WITH_INCLUDES)

# C/C++ headers get installed in ${prefix}/include/gnuradio
grinclude_HEADERS =			\
	gsm_fr_decode_ps.h		\
	gsm_fr_encode_sp.h

lib_LTLIBRARIES = libgnuradio-gsm-fr-vocoder.la

libgnuradio_gsm_fr_vocoder_la_SOURCES = \
	gsm_fr_decode_ps.cc		\
	gsm_fr_encode_sp.cc

libgnuradio_gsm_fr_vocoder_la_LIBADD =	\
	$(GNURADIO_CORE_LA)		\
	gsm/libgsm.la

libgnuradio_gsm_fr_vocoder_la_LDFLAGS =	$(NO_UNDEFINED) $(LTVERSIONFLAGS)


# SWIG interface and library
TOP_SWIG_IFILES = 			\
	gsm_full_rate.i

# Install so that they end up available as:
#   import gnuradio.vocoder.gsm_full_rate
# This ends up at:
#   ${prefix}/lib/python${python_version}/site-packages/gnuradio/vocoder
gsm_full_rate_pythondir_category =	\
	gnuradio/vocoder

# additional libraries for linking with the SWIG-generated library
gsm_full_rate_la_swig_libadd =		\
	libgnuradio-gsm-fr-vocoder.la

