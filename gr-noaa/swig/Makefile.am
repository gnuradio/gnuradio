#
# Copyright 2004,2005,2006,2008,2009,2010 Free Software Foundation, Inc.
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

TESTS =
EXTRA_DIST += $(nobase_guile_DATA)

noinst_GUILE = noaa.test

if GUILE
nobase_guile_DATA = gnuradio/noaa.scm
endif


AM_CPPFLAGS = \
	-I$(top_srcdir)/gr-noaa/lib \
	$(STD_DEFINES_AND_INCLUDES) \
	$(PYTHON_CPPFLAGS) \
	$(WITH_INCLUDES)

# ----------------------------------------------------------------
# The SWIG library
# TESTS = run_tests

TOP_SWIG_DOC_IFILES = \
	noaa_swig_doc.i

TOP_SWIG_IFILES = \
	$(TOP_SWIG_DOC_IFILES) 	\
	noaa_swig.i

BUILT_SOURCES += \
	$(TOP_SWIG_DOC_IFILES)

EXTRA_DIST += \
	$(TOP_SWIG_DOC_IFILES)

$(TOP_SWIG_DOC_IFILES):
	`echo "" > $@`

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
	noaa_hrpt_decoder.i 	\
	noaa_hrpt_deframer.i 	\
	noaa_hrpt_pll_cf.i 	\
	$(TOP_SWIG_DOC_IFILES)

if GUILE
TESTS += run_guile_tests
endif
