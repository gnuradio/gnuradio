#
# Copyright 2004,2005,2006,2007,2008 Free Software Foundation, Inc.
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

ourpythondir = $(grpythondir)/gpio
ourlibdir    = $(grpyexecdir)/gpio

AM_CPPFLAGS = $(STD_DEFINES_AND_INCLUDES) $(PYTHON_CPPFLAGS) $(WITH_INCLUDES)

SWIGPYTHONARGS = $(SWIGPYTHONFLAGS) $(STD_DEFINES_AND_INCLUDES) \
    $(WITH_SWIG_INCLUDES) $(WITH_INCLUDES)

ALL_IFILES = 				\
	$(LOCAL_IFILES)			\
	$(NON_LOCAL_IFILES)		

NON_LOCAL_IFILES = $(GNURADIO_I)

LOCAL_IFILES = 				\
	$(top_srcdir)/gr-gpio/src/lib/gpio.i				

# These files are built by SWIG.  The first is the C++ glue.
# The second is the python wrapper that loads the _gpio shared library
# and knows how to call our extensions.

BUILT_SOURCES = 			\
	gpio_swig.cc			\
	gpio_swig.py				

# This gets gpio.py installed in the right place
ourpython_PYTHON =			\
	gpio_swig.py

ourlib_LTLIBRARIES = _gpio_swig.la

# These are the source files that go into the shared library
_gpio_swig_la_SOURCES =			\
	gpio_swig.cc			\
	gpio_and_const_ss.cc		

# magic flags
_gpio_swig_la_LDFLAGS = $(NO_UNDEFINED) -module -avoid-version

# link the library against some comon swig runtime code and the 
# c++ standard library
_gpio_swig_la_LIBADD =			\
	$(PYTHON_LDFLAGS)		\
	$(GNURADIO_CORE_LA)		\
	-lstdc++			

gpio_swig.cc gpio_swig.py: $(LOCAL_IFILES) $(ALL_IFILES)
	$(SWIG) $(SWIGPYTHONARGS) -module gpio_swig -o gpio_swig.cc $(LOCAL_IFILES)

# These headers get installed in ${prefix}/include/gnuradio
grinclude_HEADERS =			\
	gpio_and_const_ss.h		

# These swig headers get installed in ${prefix}/include/gnuradio/swig
swiginclude_HEADERS = 			\
	$(LOCAL_IFILES)


# Don't distribute output of swig
dist-hook:
	@for file in $(BUILT_SOURCES); do echo $(RM) $(distdir)/$$file; done
	@for file in $(BUILT_SOURCES); do $(RM) $(distdir)/$$file; done

MOSTLYCLEANFILES = $(BUILT_SOURCES) *.pyc *~
