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

AM_CPPFLAGS = \
	$(STD_DEFINES_AND_INCLUDES) \
	$(PYTHON_CPPFLAGS) \
	$(WITH_INCLUDES) \
	-I$(top_srcdir)/gr-noaa/lib

if PYTHON
# ----------------------------------------------------------------
# The SWIG library
# TESTS = run_tests

TOP_SWIG_IFILES = \
	noaa_swig.i

# Install so that they end up available as:
#   import gnuradio.noaa
# This ends up at:
#   ${prefix}/lib/python${python_version}/site-packages/gnuradio/noaa
noaa_swig_pythondir_category = \
	gnuradio/noaa

# additional libraries for linking with the SWIG-generated library
noaa_swig_la_swig_libadd = \
	$(top_builddir)/gr-noaa/lib/libgnuradio-noaa.la

# additional Python files to be installed along with the SWIG-generated one
noaa_swig_python = \
	__init__.py

# additional SWIG files to be installed
noaa_swig_swiginclude_headers =	\
	noaa_hrpt_bit_sync.i \
	noaa_hrpt_decoder.i \
	noaa_hrpt_deframer.i \
	noaa_hrpt_pll_cf.i

include $(top_srcdir)/Makefile.swig

# add some of the variables generated inside the Makefile.swig.gen
BUILT_SOURCES = $(swig_built_sources)

# Do not distribute the output of SWIG
no_dist_files = $(swig_built_sources)
endif
