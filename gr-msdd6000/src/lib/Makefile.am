#
# Copyright 2008 Free Software Foundation, Inc.
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

# Install this stuff so that it ends up as the gnuradio.msdd module
# This usually ends up at:
#   ${prefix}/lib/python${python_version}/site-packages/gnuradio

ourpythondir = $(grpythondir)
ourlibdir    = $(grpyexecdir)

AM_CPPFLAGS = 				\
	$(STD_DEFINES_AND_INCLUDES)  	\
	$(PYTHON_CPPFLAGS) 		\
	$(WITH_INCLUDES) 		\
	$(CPPUNIT_INCLUDES)

noinst_LTLIBRARIES = libmsdd.la

libmsdd_la_SOURCES =  			\
	msdd_source_base.cc  		\
	msdd_source_c.cc  		\
	msdd_source_s.cc

# ------------------------------------------------------------------------
#  This is the swig-ish part of the Makefile.
#  It builds the msdd module which we'll load into python
# ------------------------------------------------------------------------

SWIGCPPPYTHONARGS = -fvirtual -python -modern $(PYTHON_CPPFLAGS) \
	$(STD_DEFINES_AND_INCLUDES) $(WITH_SWIG_INCLUDES) $(WITH_INCLUDES)

ALL_IFILES = 				\
	$(LOCAL_IFILES)			\
	$(NON_LOCAL_IFILES)		

NON_LOCAL_IFILES =			\
	$(top_srcdir)/gnuradio-core/src/lib/swig/gnuradio.i

LOCAL_IFILES = 				\
	$(top_srcdir)/gr-msdd6000/src/lib/msdd.i				


# These files are built by SWIG.  The first is the C++ glue.
# # The second is the python wrapper that loads the _msdd shared library
# # and knows how to call our extensions.
#
#
#
swig_built_sources =			\
	msdd.cc				\
	msdd.py	

# This gets msdd.py installed in the right place
#
ourpython_PYTHON =			\
	msdd.py


ourlib_LTLIBRARIES = _msdd.la

# These are the source files that go into the shared library
_msdd_la_SOURCES = 			\
	msdd.cc

# magic flag
_msdd_la_LDFLAGS = $(NO_UNDEFINED) -module -avoid-version 

# link the library against some comon swig runtime code and the 
# c++ standard library

_msdd_la_LIBADD = 			\
	$(GNURADIO_CORE_LA)             \
	$(PYTHON_LDFLAGS)		\
	libmsdd.la			\
	-lstdc++

msdd.cc msdd.py: msdd.i $(ALL_FILES)
	$(SWIG) $(SWIGCPPPYTHONARGS) -module msdd -o msdd.cc $(LOCAL_IFILES)

# These headers get installed in ${prefix}/include/gnuradio
grinclude_HEADERS =			\
	msdd_buffer_copy_behaviors.h	\
	msdd_source_base.h		\
	msdd_source_c.h			\
	msdd_source_s.h			

# These swig headers get installed in ${prefix}/include/gnuradio/swig
swiginclude_HEADERS = 			\
	$(LOCAL_IFILES)

MOSTLYCLEANFILES = $(BUILT_SOURCES) *.pyc *~

# Don't distribute output of swig
dist-hook:
	@for file in $(BUILT_SOURCES); do echo $(RM) $(distdir)/$$file; done
	@for file in $(BUILT_SOURCES); do $(RM) $(distdir)/$$file; done
