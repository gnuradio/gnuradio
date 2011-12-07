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

AM_CPPFLAGS = \
	-I$(top_srcdir)/gr-pager/lib \
	$(STD_DEFINES_AND_INCLUDES) \
	$(PYTHON_CPPFLAGS) \
	$(WITH_INCLUDES)

if GUILE
nobase_guile_DATA = \
	gnuradio/pager.scm
endif

noinst_GUILE  = pager.test


##############################
# SWIG interface and library

TOP_SWIG_DOC_IFILES = \
	pager_swig_doc.i

TOP_SWIG_IFILES = \
	$(TOP_SWIG_DOC_IFILES) 	\
	pager_swig.i

BUILT_SOURCES += \
	$(TOP_SWIG_DOC_IFILES)

EXTRA_DIST += \
	$(TOP_SWIG_DOC_IFILES)

$(TOP_SWIG_DOC_IFILES):
	`echo "" > $@`

# Install so that they end up available as:
#   import gnuradio.pager
# This ends up at:
#   ${prefix}/lib/python${python_version}/site-packages/gnuradio/pager
pager_swig_pythondir_category = \
	gnuradio/pager

# additional libraries for linking with the SWIG-generated library
pager_swig_la_swig_libadd = \
	$(abs_top_builddir)/gr-pager/lib/libgnuradio-pager.la

# additional SWIG files to be installed
pager_swig_swiginclude_headers = \
	pager_flex_deinterleave.i 	\
	pager_flex_frame.i 		\
	pager_flex_parse.i 		\
	pager_flex_sync.i 		\
	pager_slicer_fb.i		\
	$(TOP_SWIG_DOC_IFILES)

if GUILE
TESTS += run_guile_tests
endif