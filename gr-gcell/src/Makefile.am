#
# Copyright 2008,2009 Free Software Foundation, Inc.
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

SUBDIRS = . examples

EXTRA_DIST = run_tests.in
TESTS = run_tests


# Install the python portion so that it ends up as the gnuradio.gcell module
#   ${prefix}/lib/python${python_version}/site-packages/gnuradio

ourpythondir = $(grpythondir)
ourlibdir    = $(grpyexecdir)

AM_CPPFLAGS = $(STD_DEFINES_AND_INCLUDES) $(GCELL_INCLUDES) \
	$(PYTHON_CPPFLAGS) $(WITH_INCLUDES)

# Special swig arguments
LOCAL_SWIG_DEFINES_AND_INCLUDES = $(GCELL_INCLUDES)

# ----------------------------------------------------------------
#                        The C++ blocks
# ----------------------------------------------------------------

# the library for the C++ blocks
lib_LTLIBRARIES = libgr_gcell.la

libgr_gcell_la_SOURCES = \
	gcell_fft_vcc.cc

grinclude_HEADERS = \
	gcell_fft_vcc.h

libgr_gcell_la_LIBADD = \
	$(GNURADIO_CORE_LA) \
	$(GCELL_LA)

libgr_gcell_la_LDFLAGS = $(NO_UNDEFINED)


# ----------------------------------------------------------------
#                         SWIG stuff
# ----------------------------------------------------------------

# the library for the swig interface
ourlib_LTLIBRARIES = _gcell.la

LOCAL_IFILES = \
	$(srcdir)/gc_job_manager.i \
	$(srcdir)/gcell.i \
	$(srcdir)/gcell_fft_vcc.i

NON_LOCAL_IFILES = \
	$(GNURADIO_I)

ALL_IFILES = \
	$(LOCAL_IFILES)	\
	$(NON_LOCAL_IFILES)

swig_built_sources = \
	gcell.cc \
	gcell.py

ourpython_PYTHON = \
	gcell.py

_gcell_la_SOURCES = \
	gcell.cc

_gcell_la_LIBADD = \
	$(PYTHON_LDFLAGS) \
	libgr_gcell.la \
	-lstdc++

_gcell_la_LDFLAGS = $(NO_UNDEFINED) -module -avoid-version

_gcell_la_CXXFLAGS = @swig_CXXFLAGS@

swiginclude_HEADERS = \
	$(LOCAL_IFILES)

#gcell.cc gcell.py: $(ALL_IFILES)
#	$(SWIG) $(STD_SWIG_PYTHON_ARGS) -module gcell \
#		-o gcell.cc $(LOCAL_IFILES)

# KLUDGE: Force runtime include of gcell.d dependency file.
# This is not guaranteed to be portable, but will probably work.
# If it works, we have accurate dependencies for our swig stuff, which is good.
@am__include@ @am__quote@./gcell.d@am__quote@

gcell.py gcell.h: gcell.cc

gcell.cc : $(srcdir)/gcell.i $(GNURADIO_I)
	if $(SWIG) $(STD_SWIG_PYTHON_ARGS) -MMD -MF gcell.Td -module gcell \
		-o gcell.cc $(srcdir)/gcell.i ;\
	then if test $(host_os) = mingw32; \
	     then sed 's,\\\\,/,g' <gcell.Td >gcell.d; rm -f gcell.Td; \
	     else mv -f gcell.Td gcell.d; fi \
	else rm -f gcell.Td; exit 1; fi 


noinst_PYTHON =  \
	qa_fft.py

# ----------------------------------------------------------------

MOSTLYCLEANFILES =  \
	$(swig_built_sources) *~ *.pyc

# Don't distribute output of swig
dist-hook:
	@for file in $(swig_built_sources); do echo $(RM) $(distdir)/$$file; done
	@for file in $(swig_built_sources); do $(RM) $(distdir)/$$file; done

DISTCLEANFILES = \
	gcell.d
