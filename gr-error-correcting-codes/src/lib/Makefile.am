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
# the Free Software Foundation, Inc., 51 Franklin Street,
# Boston, MA 02110-1301, USA.
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

NON_LOCAL_IFILES = $(GNURADIO_I)

LOCAL_IFILES = \
	$(top_srcdir)/gr-error-correcting-codes/src/lib/ecc_syms_to_metrics.i \
	$(top_srcdir)/gr-error-correcting-codes/src/lib/ecc_metrics_decode_viterbi_full_block.i		\
	$(top_srcdir)/gr-error-correcting-codes/src/lib/ecc_streams_encode_convolutional.i		\
	$(top_srcdir)/gr-error-correcting-codes/src/lib/ecc_streams_encode_turbo.i			\
	$(top_srcdir)/gr-error-correcting-codes/src/lib/ecc.i

# These files are built by SWIG.  The first is the C++ glue.
# The second is the python wrapper that loads the _howto shared library
# and knows how to call our extensions.

ALL_IFILES = $(LOCAL_IFILES) $(NON_LOCAL_IFILES)

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

ecc.cc ecc.py: $(top_srcdir)/gr-error-correcting-codes/src/lib/ecc.i $(ALL_IFILES)
	$(SWIG) $(SWIGCPPPYTHONARGS) -module ecc -o ecc.cc $(top_srcdir)/gr-error-correcting-codes/src/lib/ecc.i

# Don't distribute output of swig
dist-hook:
	@for file in $(BUILT_SOURCES); do echo $(RM) $(distdir)/$$file; done
	@for file in $(BUILT_SOURCES); do $(RM) $(distdir)/$$file; done

MOSTLYCLEANFILES = $(BUILT_SOURCES) *.pyc *.loT *~
