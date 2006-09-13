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

# Install this stuff so that it ends up as the gnuradio.ezdop module
# This usually ends up at:
#   ${prefix}/lib/python${python_version}/site-packages/gnuradio

ourpythondir = $(grpythondir)
ourlibdir    = $(grpyexecdir)

EZDOP_INCLUDES = \
    -I$(top_srcdir)/ezdop/src/firmware \
    -I$(top_srcdir)/ezdop/src/host/ezdop

EZDOP_LIBS = -L$(top_builddir)/ezdop/src/host/ezdop/ -lezdop

INCLUDES = \
    $(STD_DEFINES_AND_INCLUDES) \
    $(PYTHON_CPPFLAGS) \
    $(EZDOP_INCLUDES)

SWIGPYTHONARGS = $(SWIGPYTHONFLAGS) $(STD_DEFINES_AND_INCLUDES)

ALL_IFILES = 				\
	$(LOCAL_IFILES)			\
	$(NON_LOCAL_IFILES)		

NON_LOCAL_IFILES =			\
	$(GNURADIO_I)


LOCAL_IFILES = 				\
	$(top_srcdir)/gr-ezdop/src/lib/ezdop.i				

# These files are built by SWIG.  The first is the C++ glue.
# The second is the python wrapper that loads the _ezdop shared library
# and knows how to call our extensions.

BUILT_SOURCES = 			\
	ezdop.cc			\
	ezdop.py				

# This gets ezdop.py installed in the right place
ourpython_PYTHON =			\
	ezdop.py

ourlib_LTLIBRARIES = _ezdop.la

# These are the source files that go into the shared library
_ezdop_la_SOURCES = 			\
	ezdop.cc			\
	ezdop_source_c.cc		

# magic flags
_ezdop_la_LDFLAGS = $(NO_UNDEFINED) -module -avoid-version

# link the library against some comon swig runtime code and the 
# c++ standard library
_ezdop_la_LIBADD = 			\
	$(PYTHON_LDFLAGS)		\
	$(GNURADIO_CORE_LIBS)		\
	$(EZDOP_LIBS)			\
	-lstdc++			

ezdop.cc ezdop.py: $(ALL_IFILES)
	$(SWIG) $(SWIGPYTHONARGS) -module ezdop -o ezdop.cc $(LOCAL_IFILES)

# These headers get installed in ${prefix}/include/gnuradio
grinclude_HEADERS =			\
	ezdop_source_c.h


# These swig headers get installed in ${prefix}/include/gnuradio/swig
swiginclude_HEADERS = 			\
	$(LOCAL_IFILES)

MOSTLYCLEANFILES = $(BUILT_SOURCES) *.pyc

# Don't distribute output of swig
dist-hook:
	@for file in $(BUILT_SOURCES); do echo $(RM) $(distdir)/$$file; done
	@for file in $(BUILT_SOURCES); do $(RM) $(distdir)/$$file; done
