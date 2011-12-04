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
include $(top_srcdir)/Makefile.swig

TESTS =

AM_CPPFLAGS = \
	-I$(top_srcdir)/gruel/src/lib \
	$(STD_DEFINES_AND_INCLUDES) \
	$(PYTHON_CPPFLAGS) \
	$(WITH_INCLUDES)

##############################
# SWIG interface and library
TOP_SWIG_IFILES = \
	pmt_swig_doc.i \
	pmt_swig.i

TOP_SWIG_DOC_IFILES = \
	pmt_swig_doc.i

BUILT_SOURCES += \
	$(TOP_SWIG_DOC_IFILES)

EXTRA_DIST += \
	$(TOP_SWIG_DOC_IFILES)

$(TOP_SWIG_DOC_IFILES):
	`echo "" > $@`

# Install so that they end up available as:
#   import gruel.pmt
# This ends up at:
#   ${prefix}/lib/python${python_version}/site-packages/gruel/pmt
pmt_swig_pythondir_category = \
	gruel/pmt

# additional libraries for linking with the SWIG-generated library
pmt_swig_la_swig_libadd = \
	$(abs_top_builddir)/gruel/src/lib/pmt/libpmt.la

# additional SWIG files to be installed
pmt_swig_swiginclude_headers =  \
	gr_intrusive_ptr.i \
	$(TOP_SWIG_DOC_IFILES)

