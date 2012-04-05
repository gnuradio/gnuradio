#
# Copyright 2012 Free Software Foundation, Inc.
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

noinst_GUILE = fcd.test

if GUILE
nobase_guile_DATA = gnuradio/fcd_swig.scm
endif


AM_CPPFLAGS = \
	-I$(top_srcdir)/gr-fcd/include/fcd \
	-I$(top_srcdir)/gr-audio/include \
	$(STD_DEFINES_AND_INCLUDES) \
	$(PYTHON_CPPFLAGS) \
	$(WITH_INCLUDES)

# ----------------------------------------------------------------
# The SWIG library
# TESTS = run_tests

fcd_swig_swig_args = $(FCD_CPPFLAGS)

TOP_SWIG_DOC_IFILES = \
	fcd_swig_doc.i

TOP_SWIG_IFILES = \
	$(TOP_SWIG_DOC_IFILES) 	\
	fcd_swig.i

BUILT_SOURCES += \
	$(TOP_SWIG_DOC_IFILES)

EXTRA_DIST += \
	$(TOP_SWIG_DOC_IFILES)

$(TOP_SWIG_DOC_IFILES):
	`echo "" > $@`

# Install so that they end up available as:
#   import gnuradio.fcd
# This ends up at:
#   ${prefix}/lib/python${python_version}/site-packages/gnuradio/fcd
fcd_swig_pythondir_category = \
	gnuradio/fcd

# additional libraries for linking with the SWIG-generated library
fcd_swig_la_swig_libadd = \
	$(top_builddir)/gr-fcd/lib/libgnuradio-fcd.la

# additional SWIG files to be installed
fcd_swig_swiginclude_headers =	\
	$(TOP_SWIG_DOC_IFILES)

if GUILE
TESTS += run_guile_tests
endif
