#
# Copyright 2004,2005,2006 Free Software Foundation, Inc.
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

include Makefile.gen

# Install this stuff so that it ends up as the gnuradio.trellis module
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
	$(GNURADIO_I)			


LOCAL_IFILES = 				\
	$(top_srcdir)/gr-trellis/src/lib/trellis.i			


# These files are built by SWIG.  The first is the C++ glue.
# The second is the python wrapper that loads the _trellis shared library
# and knows how to call our extensions.

BUILT_SOURCES = 			\
	trellis.cc			\
	trellis.py				

# This gets trellis.py installed in the right place
ourpython_PYTHON =			\
	trellis.py

ourlib_LTLIBRARIES = _trellis.la

# These are the source files that go into the shared library
_trellis_la_SOURCES = 			\
	trellis.cc			\
        fsm.cc				\
        quicksort_index.cc		\
        base.cc				\
        interleaver.cc			\
        trellis_calc_metric.cc		\
        trellis_permutation.cc		\
	trellis_siso_f.cc		\
	trellis_siso_combined_f.cc	\
	$(GENERATED_CC)                 

# magic flags
_trellis_la_LDFLAGS = $(NO_UNDEFINED) -module -avoid-version

# link the library against some comon swig runtime code and the 
# c++ standard library
_trellis_la_LIBADD = 			\
	$(PYTHON_LDFLAGS)		\
	$(GNURADIO_CORE_LIBS)		\
	-lstdc++			

trellis.cc trellis.py: $(ALL_IFILES) $(grinclude_HEADERS)
	$(SWIG) $(SWIGPYTHONARGS) -module trellis -o trellis.cc $(LOCAL_IFILES)

# These headers get installed in ${prefix}/include/gnuradio
grinclude_HEADERS =			\
        fsm.h				\
        quicksort_index.h		\
        base.h				\
        interleaver.h			\
        trellis_metric_type.h		\
        trellis_calc_metric.h		\
        trellis_permutation.h		\
        trellis_siso_type.h		\
	trellis_siso_f.h		\
	trellis_siso_combined_f.h		\
	$(GENERATED_H)                  


# These swig headers get installed in ${prefix}/include/gnuradio/swig
swiginclude_HEADERS = 			\
	$(LOCAL_IFILES)			\
	$(GENERATED_I)			\
	fsm.i				\
	interleaver.i			\
	trellis_permutation.i		\
	trellis_siso_f.i		\
	trellis_siso_combined_f.i	\
	trellis_generated.i



MOSTLYCLEANFILES = $(BUILT_SOURCES) *.pyc

# Don't distribute output of swig
dist-hook:
	@for file in $(BUILT_SOURCES); do echo $(RM) $(distdir)/$$file; done
	@for file in $(BUILT_SOURCES); do $(RM) $(distdir)/$$file; done
