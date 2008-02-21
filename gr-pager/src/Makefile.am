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

EXTRA_DIST = \
    run_tests.in \
    $(bin_SCRIPTS)

TESTS = \
    run_tests

bin_SCRIPTS = \
    usrp_flex.py \
    usrp_flex_all.py \
    usrp_flex_band.py

noinst_PYTHON = \
    qa_pager.py

# Install this stuff so that it ends up as the gnuradio.pgr module
# This usually ends up at:
#   ${prefix}/lib/python${python_version}/site-packages/gnuradio/pager

ourpythondir = $(grpythondir)/pager
ourlibdir    = $(grpyexecdir)/pager

AM_CPPFLAGS = $(STD_DEFINES_AND_INCLUDES) $(PYTHON_CPPFLAGS) $(WITH_INCLUDES)

SWIGPYTHONARGS = $(SWIGPYTHONFLAGS) $(STD_DEFINES_AND_INCLUDES) \
	$(WITH_SWIG_INCLUDES) $(WITH_INCLUDES)

ALL_IFILES = \
    $(LOCAL_IFILES) \
    $(NON_LOCAL_IFILES)	\
    pager_flex_frame.i

NON_LOCAL_IFILES = \
    $(GNURADIO_I)

LOCAL_IFILES = \
    $(top_srcdir)/gr-pager/src/pager.i

# These files are built by SWIG.  The first is the C++ glue.
# The second is the python wrapper that loads the _howto shared library
# and knows how to call our extensions.

BUILT_SOURCES = \
    pager_swig.cc \
    pager_swig.py				

# This gets imported Python objects installed in the right place
ourpython_PYTHON = \
    __init__.py \
    pager_swig.py \
    flex_demod.py
        
ourlib_LTLIBRARIES = _pager_swig.la

# These are the source files that go into the shared library
_pager_swig_la_SOURCES = \
    pager_swig.cc \
    pager_flex_frame.cc \
    pager_slicer_fb.cc \
    pager_flex_sync.cc \
    pager_flex_deinterleave.cc \
    pager_flex_parse.cc \
    pageri_bch3221.cc \
    pageri_flex_modes.cc \
    pageri_util.cc
    # Additional source modules here

# magic flags
_pager_swig_la_LDFLAGS = $(NO_UNDEFINED) -module -avoid-version

# link the library against the c++ standard library
_pager_swig_la_LIBADD = \
    $(PYTHON_LDFLAGS) \
    $(GNURADIO_CORE_LA) \
    -lstdc++			

pager_swig.cc pager_swig.py: $(ALL_IFILES)
	$(SWIG) $(SWIGPYTHONARGS) -module pager_swig -o pager_swig.cc $(LOCAL_IFILES)

# These headers get installed in ${prefix}/include/gnuradio
grinclude_HEADERS = \
    pager_slicer_fb.h \
    pager_flex_sync.h \
    pager_flex_deinterleave.h \
    pager_flex_parse.h \
    pager_flex_frame.h \
    pageri_bch3221.h \
    pageri_flex_modes.h \
    pageri_util.h
    # Additional header files here

# These swig headers get installed in ${prefix}/include/gnuradio/swig
swiginclude_HEADERS = \
    $(LOCAL_IFILES) \
    pager_flex_frame.i

MOSTLYCLEANFILES = $(BUILT_SOURCES) *.pyc *~ *.tmp

# Don't distribute output of swig
dist-hook:
	@for file in $(BUILT_SOURCES); do echo $(RM) $(distdir)/$$file; done
	@for file in $(BUILT_SOURCES); do $(RM) $(distdir)/$$file; done
