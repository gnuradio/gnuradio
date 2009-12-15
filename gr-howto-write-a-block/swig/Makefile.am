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

AM_CPPFLAGS += -I$(top_srcdir)/lib

if PYTHON
###################################
# SWIG Python interface and library

TOP_SWIG_IFILES = 		\
	howto.i

# Install so that they end up available as:
#   import gnuradio.howto
# This ends up at:
#   ${prefix}/lib/python${python_version}/site-packages/gnuradio
howto_pythondir_category = gnuradio/howto

howto_la_swig_libadd =		\
	$(top_builddir)/lib/libgnuradio-howto.la

include $(top_srcdir)/Makefile.swig

# add some of the variables generated inside the Makefile.swig.gen
BUILT_SOURCES = $(swig_built_sources)

# Do not distribute the output of SWIG
no_dist_files = $(swig_built_sources)

# additional SWIG files to be installed
howto_swiginclude_headers =	\
	howto_square_ff.i \
	howto_square2_ff.i

endif
