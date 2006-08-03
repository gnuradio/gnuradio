#
# Copyright 2004,2005 Free Software Foundation, Inc.
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

LIBS += $(GNURADIO_CORE_LIBS)

# Install this stuff so that it ends up as the gnuradio.radar module
# This usually ends up at:
#   ${prefix}/lib/python${python_version}/site-packages/gnuradio

ourpythondir = $(grpythondir)
ourlibdir    = $(grpyexecdir)

INCLUDES = $(STD_DEFINES_AND_INCLUDES) $(PYTHON_CPPFLAGS)

SWIGPYTHONARGS = $(SWIGPYTHONFLAGS) $(STD_DEFINES_AND_INCLUDES)

ALL_IFILES = 				\
	$(LOCAL_IFILES)			\
	$(NON_LOCAL_IFILES)		

NON_LOCAL_IFILES =			\
	$(top_srcdr)/gnuradio-core/src/lib/swig/gnuradio.i


LOCAL_IFILES = 				


# These files are built by SWIG.  The first is the C++ glue.
# The second is the python wrapper that loads the _howto shared library
# and knows how to call our extensions.

BUILT_SOURCES = 			

lib_LTLIBRARIES = libradar.la

libradar_la_SOURCES = 			\
	time_series.h			\
	time_series.cc			\
	simulation.h			\
	simulation.cc			

bin_PROGRAMS = 				\
	xambi				\
	eb-xambi			\
	sim-airplane			\
	sim-airplane2			

xambi_SOURCES 	= xambi.cc
xambi_LDADD	= libradar.la

eb_xambi_SOURCES = eb-xambi.cc
eb_xambi_LDADD	 = libradar.la

sim_airplane_SOURCES = sim-airplane.cc
sim_airplane_LDADD = libradar.la

sim_airplane2_SOURCES = sim-airplane2.cc
sim_airplane2_LDADD = libradar.la

# This gets howto.py installed in the right place
# ourpython_PYTHON =			

#ourlib_LTLIBRARIES = _howto.la

# These are the source files that go into the shared library
#_howto_la_SOURCES = 			\
#	howto.cc			\
#	howto_square_ff.cc		\
#	howto_square2_ff.cc		

# magic flags
#_howto_la_LDFLAGS = $(NO_UNDEFINED) -module -avoid-version

# link the library against some comon swig runtime code and the 
# c++ standard library
#_howto_la_LIBADD = 			\
#	$(PYTHON_LDFLAGS)		\
#	-lstdc++			

#howto.cc howto.py: howto.i $(ALL_IFILES)
#	$(SWIG) $(SWIGPYTHONARGS) -module howto -o howto.cc $<

# These headers get installed in ${prefix}/include/gnuradio
grinclude_HEADERS =			


# These swig headers get installed in ${prefix}/include/gnuradio/swig
swiginclude_HEADERS = 			\
	$(LOCAL_IFILES)


MOSTLYCLEANFILES = $(BUILT_SOURCES) *.pyc


# Don't distribute output of swig
dist-hook:
	@for file in $(BUILT_SOURCES); do echo $(RM) $(distdir)/$$file; done
	@for file in $(BUILT_SOURCES); do $(RM) $(distdir)/$$file; done
