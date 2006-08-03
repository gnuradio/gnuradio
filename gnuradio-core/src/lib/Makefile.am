#
# Copyright 2001,2004 Free Software Foundation, Inc.
# 
# This file is part of GNU Radio
# 
# GNU Radio is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2, or (at your option)
# any later version.
# 
# GNU Radio is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with GNU Radio; see the file COPYING.  If not, write to
# the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
# Boston, MA 02111-1307, USA.
# 

include $(top_srcdir)/Makefile.common

## Process this file with automake to produce Makefile.in

# We've got to build . before swig
# FIXME add atsc back
SUBDIRS = missing runtime filter general g72x reed-solomon omnithread io . swig

# generate libgnuradio-core.la from the convenience libraries in subdirs

lib_LTLIBRARIES = libgnuradio-core.la libgnuradio-core-qa.la

libgnuradio_core_la_SOURCES = bug_work_around_6.cc
libgnuradio_core_la_LDFLAGS = $(NO_UNDEFINED) -version-info 0:0:0

libgnuradio_core_qa_la_SOURCES = bug_work_around_6.cc
libgnuradio_core_qa_la_LDFLAGS = $(NO_UNDEFINED) -version-info 0:0:0 \
				$(LIBGNURADIO_CORE_EXTRA_LDFLAGS)

FIXME = \
	atsc/libatsc.la			

libgnuradio_core_la_LIBADD  = 		\
	filter/libfilter.la		\
	g72x/libccitt.la		\
	general/libgeneral.la		\
	io/libio.la			\
	missing/libmissing.la		\
	omnithread/libomnithread.la	\
	reed-solomon/librs.la		\
	runtime/libruntime.la		\
	$(FFTW3F_LIBS)

libgnuradio_core_qa_la_LIBADD  = 	\
	filter/libfilter-qa.la		\
	general/libgeneral-qa.la	\
	runtime/libruntime-qa.la	\
	missing/libmissing.la		\
	libgnuradio-core.la		\
	$(CPPUNIT_LIBS)			
