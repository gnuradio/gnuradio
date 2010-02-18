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
	-I$(top_srcdir)/gr-pager/lib

##############################
# SWIG interface and library
TOP_SWIG_IFILES = \
	pager_swig.i

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
	pager_flex_deinterleave.i \
	pager_flex_frame.i \
	pager_flex_parse.i \
	pager_flex_sync.i \
	pager_slicer_fb.i

include $(top_srcdir)/Makefile.swig

# add some of the variables generated inside the Makefile.swig.gen
BUILT_SOURCES = $(swig_built_sources)

# Do not distribute the output of SWIG
no_dist_files = $(swig_built_sources)
