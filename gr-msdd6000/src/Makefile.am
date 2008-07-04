#
# Copyright 2007,2008 Free Software Foundation, Inc.
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

# Install this stuff so that it ends up as the gnuradio.howto module
# This usually ends up at:
#   ${prefix}/lib/python${python_version}/site-packages/gnuradio

ourpythondir = $(grpythondir)
ourlibdir    = $(grpyexecdir)


#EXTRA_DIST = run_tests.in

#TESTS = run_tests


#INCLUDES = $(STD_DEFINES_AND_INCLUDES) $(PYTHON_CPPFLAGS)

AM_CPPFLAGS = $(STD_DEFINES_AND_INCLUDES) $(PYTHON_CPPFLAGS) \
	$(WITH_INCLUDES)


SWIGPYTHONARGS = $(SWIGPYTHONFLAGS) $(STD_DEFINES_AND_INCLUDES) \
	-I/usr/include $(WITH_SWIG_INCLUDES) $(WITH_INCLUDES)

NON_LOCAL_IFILES =			\
	$(GNURADIO_I)


LOCAL_IFILES = 				\
	$(top_srcdir)/gr-msdd6000/src/msdd.i				


ALL_IFILES = 				\
	$(LOCAL_IFILES)			\
	$(NON_LOCAL_IFILES)		


# These files are built by SWIG.  The first is the C++ glue.
# The second is the python wrapper that loads the _howto shared library
# and knows how to call our extensions.

BUILT_SOURCES = 			\
	msdd.cc			\
	msdd.py				

# This gets howto.py installed in the right place
ourpython_PYTHON =			\
	msdd.py

ourlib_LTLIBRARIES = _msdd.la

# These are the source files that go into the shared library
_msdd_la_SOURCES = 			\
	msdd.cc			\
	msdd_source_base.cc	\
	msdd_source_c.cc	\
	msdd_source_simple.cc	\
	msdd_source_s.cc	\
	msdd6000.cc

# magic flag
_msdd_la_LDFLAGS = $(NO_UNDEFINED) -module -avoid-version 
#-lfftw3 -lfftw3f -lfann 

# link the library against some comon swig runtime code and the 
# c++ standard library
_msdd_la_LIBADD = 			\
	$(PYTHON_LDFLAGS)		\
	-lstdc++
# -L/usr/lib/libfftw3f.so.3.1.2

msdd.cc msdd.py: $(LOCAL_IFILES) $(ALL_IFILES) 
	$(SWIG) $(SWIGPYTHONARGS) -module msdd -o msdd.cc $(LOCAL_IFILES)

# These headers get installed in ${prefix}/include/gnuradio
grinclude_HEADERS =			\
	msdd_source_base.h		\
	msdd_source_c.h			\
	msdd_source_simple.h			\
	msdd_source_s.h		\
	msdd_buffer_copy_behaviors.h \
	msdd6000.h


# These swig headers get installed in ${prefix}/include/gnuradio/swig
swiginclude_HEADERS = 			\
	$(LOCAL_IFILES)


MOSTLYCLEANFILES = $(BUILT_SOURCES) *.pyc

# Don't distribute output of swig
dist-hook:
	@for file in $(BUILT_SOURCES); do echo $(RM) $(distdir)/$$file; done
	@for file in $(BUILT_SOURCES); do $(RM) $(distdir)/$$file; done
