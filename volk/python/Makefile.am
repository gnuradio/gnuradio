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

AM_CPPFLAGS = $(STD_DEFINES_AND_INCLUDES) $(PYTHON_CPPFLAGS)
SWIGPYTHONARGS = $(SWIGPYTHONFLAGS) $(SWIGGRFLAGS)

BUILT_SOURCES = $(swig_built_sources)

MOSTLYCLEANFILES = $(BUILT_SOURCES) *.pyc

# Don't distribute the output of swig
dist-hook:
	@for file in $(swig_built_sources); do echo $(RM) $(distdir)/$$file; done
	@for file in $(swig_built_sources); do $(RM) $(distdir)/$$file; done


# if we're not using python, ignore the rest...

TESTS = 


ALL_IFILES = \
	$(LOCAL_IFILES)	\
	$(NON_LOCAL_IFILES)

NON_LOCAL_IFILES = \
	$(GNURADIO_CORE_INCLUDEDIR)/swig/gnuradio.i

LOCAL_IFILES = \
	libvector.i \
	libvector_square_ff.i


# These files are built by SWIG.  The first is the C++ glue.
# The second is the python wrapper that loads the _libvector shared library
# and knows how to call our extensions.

swig_built_sources = \
	libvector_swig.cc \
	libvector_swig.py

# This gets libvector.py installed in the right place.
# Any hand-written python should be listed here too.
ourpython_PYTHON = \
	__init__.py \
	libvector_swig.py

# Python QA code gets listed here.  It's not installed.
noinst_PYTHON = 

# This library contains the swig generated glue
ourpyexec_LTLIBRARIES = _libvector_swig.la

# These are the source files that go into the shared library
_libvector_swig_la_SOURCES = \
	libvector_swig.cc

# Dummy C++ source to cause C++ linking
nodist_EXTRA__libvector_swig_la_SOURCES = dummy.cc

# magic flags
_libvector_swig_la_LDFLAGS = $(NO_UNDEFINED) -module -avoid-version

# Link the swig generated code against our library and some python magic.
_libvector_swig_la_LIBADD = \
	../lib/liblibvector.la \
	$(PYTHON_LDFLAGS)


libvector_swig.cc libvector_swig.py: $(LOCAL_IFILES) $(ALL_IFILES)
	$(SWIG) $(SWIGPYTHONARGS) -module libvector_swig -o libvector_swig.cc $(srcdir)/libvector.i


# These swig headers get installed in ${prefix}/include/<component>/swig
ourswiginclude_HEADERS = \
	$(LOCAL_IFILES)
