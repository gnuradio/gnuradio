# Copyright 2001,2004,2009,2010 Free Software Foundation, Inc.
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

## Process this file with automake to produce Makefile.in

# We've got to build . before swig
SUBDIRS = missing runtime filter viterbi general gengen reed-solomon io hier . swig

AM_CPPFLAGS = $(STD_DEFINES_AND_INCLUDES) $(CPPUNIT_INCLUDES) $(WITH_INCLUDES)

# generate libgnuradio-core.la from the convenience libraries in subdirs

lib_LTLIBRARIES = libgnuradio-core.la
noinst_LTLIBRARIES = libgnuradio-core-qa.la

libgnuradio_core_la_SOURCES = bug_work_around_6.cc
libgnuradio_core_la_LDFLAGS = $(NO_UNDEFINED) $(LTVERSIONFLAGS)

libgnuradio_core_qa_la_SOURCES = bug_work_around_6.cc
libgnuradio_core_qa_la_LDFLAGS = $(NO_UNDEFINED) -version-info 0:0:0 \
				 $(LIBGNURADIO_CORE_EXTRA_LDFLAGS)


libgnuradio_core_la_LIBADD  = 		\
	filter/libfilter.la		\
	viterbi/libviterbi.la		\
	general/libgeneral.la		\
	gengen/libgengen.la		\
	io/libio.la			\
	missing/libmissing.la		\
	reed-solomon/librs.la		\
	runtime/libruntime.la		\
	hier/libhier.la			\
	$(GRUEL_LA)			\
	$(VOLK_LA)			\
	$(FFTW3F_LIBS)			\
	$(GSL_LIBS)			\
	$(CBLAS_LIBS)			\
	$(BOOST_FILESYSTEM_LIB)

libgnuradio_core_qa_la_LIBADD  = 	\
	filter/libfilter-qa.la		\
	general/libgeneral-qa.la	\
	runtime/libruntime-qa.la	\
	libgnuradio-core.la		\
	$(CPPUNIT_LIBS)			

bin_PROGRAMS = gnuradio-config-info
gnuradio_config_info_SOURCES = gnuradio-config-info.cc
gnuradio_config_info_LDADD = libgnuradio-core.la \
	$(BOOST_LDFLAGS) \
	$(BOOST_PROGRAM_OPTIONS_LIB) \
	$(BOOST_FILESYSTEM_LIB)
