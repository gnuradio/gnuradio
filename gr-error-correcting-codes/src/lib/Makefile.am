#
# Copyright 2006 Free Software Foundation, Inc.
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

SUBDIRS = libecc

# Install this stuff so that it ends up as the gnuradio.und module
# This usually ends up at:
#   ${prefix}/lib/python${python_version}/site-packages/gnuradio

ourpythondir = $(grpythondir)
ourlibdir    = $(grpythondir)

INCLUDES = $(PYTHON_CPPFLAGS) $(STD_DEFINES_AND_INCLUDES)

SWIGCPPPYTHONARGS = -fvirtual -python -modern $(PYTHON_CPPFLAGS) \
	$(STD_DEFINES_AND_INCLUDES)

ALL_IFILES = $(LOCAL_IFILES) $(NON_LOCAL_IFILES)

NON_LOCAL_IFILES = $(GNURADIO_I)

LOCAL_IFILES = 						\
	ecc_syms_to_metrics.i				\
	ecc_metrics_decode_viterbi_full_block.i		\
	ecc_streams_encode_convolutional.i		\
	ecc_streams_encode_turbo.i			\
	ecc.i

# These files are built by SWIG.  The first is the C++ glue.
# The second is the python wrapper that loads the _howto shared library
# and knows how to call our extensions.

BUILT_SOURCES = 	\
	ecc.cc		\
	ecc.py		

# This gets ecc.py installed in the right place
ourpython_PYTHON =	\
	__init__.py	\
	ecc.py

ourlib_LTLIBRARIES = _ecc.la

# These are the source files that go into the shared library
_ecc_la_SOURCES = 					\
	ecc_syms_to_metrics.cc				\
	ecc_metrics_decode_viterbi_full_block.cc	\
	ecc_streams_encode_convolutional.cc		\
	ecc_streams_encode_turbo.cc			\
	ecc.cc

# magic flags
_ecc_la_LDFLAGS = $(NO_UNDEFINED) -module -avoid-version

# link the library against the c++ standard library
_ecc_la_LIBADD = 		\
	libecc/libecc.la	\
	$(PYTHON_LDFLAGS)	\
	$(GNURADIO_CORE_LIBS)	\
	-lstdc++

# These headers get installed in ${prefix}/include/gnuradio
grinclude_HEADERS =					\
	ecc_syms_to_metrics.h				\
	ecc_metrics_decode_viterbi_full_block.h		\
	ecc_streams_encode_convolutional.h		\
	ecc_streams_encode_turbo.h

# These swig headers get installed in ${prefix}/include/gnuradio/swig
swiginclude_HEADERS = $(LOCAL_IFILES)

ecc.cc ecc.py: ecc.i $(ALL_IFILES)
	$(SWIG) $(SWIGCPPPYTHONARGS) -module ecc -o ecc.cc $<

MOSTLYCLEANFILES = $(BUILT_SOURCES) *.pyc *.loT *~

CONFIG_CLEAN_FILES = *.in
