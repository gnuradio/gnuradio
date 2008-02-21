#
# Copyright 2004,2005,2006,2008 Free Software Foundation, Inc.
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

# Install this stuff so that it ends up as the gnuradio.howto module
# This usually ends up at:
#   ${prefix}/lib/python${python_version}/site-packages/gnuradio

ourpythondir = $(grpythondir)
ourlibdir    = $(grpyexecdir)

AM_CPPFLAGS = $(STD_DEFINES_AND_INCLUDES) $(PYTHON_CPPFLAGS) $(WITH_INCLUDES)

SWIGPYTHONARGS = $(SWIGPYTHONFLAGS) $(SWIGGRFLAGS) $(WITH_SWIG_INCLUDES) \
	$(WITH_INCLUDES)

ALL_IFILES = 				\
	$(LOCAL_IFILES)			\
	$(NON_LOCAL_IFILES)		

NON_LOCAL_IFILES =			\
	$(GNURADIO_CORE_INCLUDEDIR)/swig/gnuradio.i


LOCAL_IFILES = 				\
	$(top_srcdir)/src/lib/howto.i				

# These files are built by SWIG.  The first is the C++ glue.
# The second is the python wrapper that loads the _howto shared library
# and knows how to call our extensions.

BUILT_SOURCES = 			\
	howto.cc			\
	howto.py				

# This gets howto.py installed in the right place
ourpython_PYTHON =			\
	howto.py

ourlib_LTLIBRARIES = _howto.la

# These are the source files that go into the shared library
_howto_la_SOURCES = 			\
	howto.cc			\
	howto_square_ff.cc		\
	howto_square2_ff.cc		

# magic flags
_howto_la_LDFLAGS = $(NO_UNDEFINED) -module -avoid-version

# link the library against some comon swig runtime code and the 
# c++ standard library
_howto_la_LIBADD = 			\
	$(PYTHON_LDFLAGS)		\
	-lstdc++			

howto.cc howto.py: $(LOCAL_IFILES) $(ALL_IFILES)
	$(SWIG) $(SWIGPYTHONARGS) -module howto -o howto.cc $(LOCAL_IFILES)

# These headers get installed in ${prefix}/include/gnuradio
grinclude_HEADERS =			\
	howto_square_ff.h		\
	howto_square2_ff.h		


# These swig headers get installed in ${prefix}/include/gnuradio/swig
swiginclude_HEADERS = 			\
	$(LOCAL_IFILES)


MOSTLYCLEANFILES = $(BUILT_SOURCES) *.pyc

# Don't distribute output of swig
dist-hook:
	@for file in $(BUILT_SOURCES); do echo $(RM) $(distdir)/$$file; done
	@for file in $(BUILT_SOURCES); do $(RM) $(distdir)/$$file; done
