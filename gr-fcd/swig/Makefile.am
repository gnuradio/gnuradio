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


AM_CPPFLAGS += -I$(top_srcdir)/lib 

###################################
# SWIG interfaces and libraries

TOP_SWIG_IFILES = \
    fcd_swig.i

# Install so that they end up available as:
#   import fcd
# This ends up at:
#   ${prefix}/lib/python${python_version}/site-packages/$(modname)
fcd_swig_pythondir_category = $(modname)

# additional libraries for linking with the SWIG-generated library
fcd_swig_la_swig_libadd = \
	$(top_builddir)/lib/libgnuradio-fcd.la

# additional SWIG files to be installed
fcd_swiginclude_headers = \
	fcd_source_c.i


EXTRA_DIST = $(fcd_swiginclude_headers)
