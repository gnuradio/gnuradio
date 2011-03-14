#
# Copyright 2001,2003,2004,2005,2006,2007,2008,2009,2010 Free Software Foundation, Inc.
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
include $(top_srcdir)/Makefile.swig

AM_CPPFLAGS = -I$(srcdir) $(STD_DEFINES_AND_INCLUDES) $(PYTHON_CPPFLAGS) \
	 $(WITH_INCLUDES)

TOP_SWIG_IFILES =		\
	pmt.i


# SWIG headers get installed in ${prefix}/include/gnuradio/swig
swiginclude_HEADERS =			\
	pmt.i			

# SWIG headers that get installed in ${prefix}/include/gnuradio/swig/...
nobase_swiginclude_HEADERS = 


# special install for this top-level Python script which includes all
# of the split Python libraries.
ourpythondir = $(pythondir)/gruel
ourpython_PYTHON =  \
    pmt.py      \
    __init__.py

# ----------------------------------------------------------------
# FIXME As of swig 1.3.31, this still seems to be required...
# gnuradio_swig_bug_workaround.h : gnuradio_core_runtime.py $(srcdir)/gen-swig-bug-fix
#	$(PYTHON) $(srcdir)/gen-swig-bug-fix python/gnuradio_core_runtime.cc $@

EXTRA_DIST += gen-swig-bug-fix

# C/C++ headers get installed in ${prefix}/include/gnuradio
#grinclude_HEADERS = gnuradio_swig_bug_workaround.h
grinclude_HEADERS = 


# Install so that they end up available as:
#   import gnuradio.gr
# This ends up at:
#   ${prefix}/lib/python${python_version}/site-packages/gnuradio/gr
pmt_pythondir_category = gruel

# additional libraries for linking with each SWIG-generated library
pmt_la_swig_libadd = $(GRUEL_LA)

SWIG_GUILE_FLAGS += -DIN_GNURADIO_CORE



