#
# Copyright 2010 Free Software Foundation, Inc.
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


# Set the install location of any python scripts to /volk
volkpythondir = $(pythondir)/volk

volkpython_PYTHON = \
	__init__.py
