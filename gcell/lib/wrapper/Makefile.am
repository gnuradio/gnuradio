#
# Copyright 2008 Free Software Foundation, Inc.
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
# You should have received a copy of the GNU General Public License along
# with this program; if not, write to the Free Software Foundation, Inc.,
# 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
#

include $(top_srcdir)/Makefile.common

AM_CPPFLAGS = $(DEFINES) $(GCELL_INCLUDES) $(FFTW3F_CFLAGS) $(WITH_INCLUDES)

noinst_LTLIBRARIES = libwrapper.la libwrapper-qa.la

#
# generate libtool.lo's that contain embedded SPU executables
#
gcell_all.lo: ../spu/gcell_all
	$(GCELL_EMBEDSPU_LIBTOOL) $< $@ 

gcell_general_qa.lo: ../spu/gcell_general_qa
	$(GCELL_EMBEDSPU_LIBTOOL) $< $@



# The primary library

libwrapper_la_SOURCES = \
	gcp_fft_1d_r2.cc

libwrapper_la_LIBADD = \
	gcell_all.lo


# The QA library

libwrapper_qa_la_SOURCES = \
	qa_gcell_general.cc \
	qa_gcell_wrapper.cc \
	qa_gcp_fft_1d_r2.cc

libwrapper_qa_la_LIBADD = \
	gcell_general_qa.lo \
	-lfftw3f

# Headers

# Moved to include/gcell
#gcellinclude_HEADERS = \
#	gcp_fft_1d_r2.h

noinst_HEADERS = \
	qa_gcell_general.h \
	qa_gcell_wrapper.h \
	qa_gcp_fft_1d_r2.h


CLEANFILES = gcell_all.lo gcell_general_qa.lo

