#
# Copyright 2004,2005,2008 Free Software Foundation, Inc.
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

SUBDIRS = gsm .

# Install this stuff so that it ends up as the gnuradio.vocoder module
# This usually ends up at:
#   ${prefix}/lib/python${python_version}/site-packages/gnuradio/vocoder

ourpythondir = $(grpythondir)/vocoder
ourlibdir    = $(grpyexecdir)/vocoder

AM_CPPFLAGS = $(STD_DEFINES_AND_INCLUDES) $(PYTHON_CPPFLAGS) $(WITH_INCLUDES)

SWIGPYTHONARGS = $(SWIGPYTHONFLAGS) $(STD_DEFINES_AND_INCLUDES) \
	$(WITH_SWIG_INCLUDES) $(WITH_INCLUDES)

ALL_IFILES = 				\
	$(LOCAL_IFILES)			\
	$(NON_LOCAL_IFILES)		

NON_LOCAL_IFILES =			\
	$(GNURADIO_I)

LOCAL_IFILES = 				\
	$(top_srcdir)/gr-gsm-fr-vocoder/src/lib/gsm_full_rate.i

# These files are built by SWIG.  The first is the C++ glue.
# The second is the python wrapper that loads the _howto shared library
# and knows how to call our extensions.

BUILT_SOURCES = 			\
	gsm_full_rate.cc		\
	gsm_full_rate.py				

# This gets gsm_full_rate.py installed in the right place
ourpython_PYTHON =			\
	gsm_full_rate.py

ourlib_LTLIBRARIES = _gsm_full_rate.la

# These are the source files that go into the shared library
_gsm_full_rate_la_SOURCES = 		\
	gsm_fr_decode_ps.cc		\
	gsm_fr_encode_sp.cc		\
	gsm_full_rate.cc		


# magic flags
_gsm_full_rate_la_LDFLAGS = $(NO_UNDEFINED) -module -avoid-version

# link the library against the gsm library and the
# c++ standard library
_gsm_full_rate_la_LIBADD = 	\
	gsm/libgsm.la		\
	$(PYTHON_LDFLAGS)	\
	$(GNURADIO_CORE_LA)	\
	-lstdc++			

gsm_full_rate.cc gsm_full_rate.py: gsm_full_rate.i $(ALL_IFILES)
	$(SWIG) $(SWIGPYTHONARGS) -module gsm_full_rate -o gsm_full_rate.cc $(LOCAL_IFILES)

# These headers get installed in ${prefix}/include/gnuradio
grinclude_HEADERS =			\
	gsm_fr_decode_ps.h		\
	gsm_fr_encode_sp.h		

# These swig headers get installed in ${prefix}/include/gnuradio/swig
swiginclude_HEADERS = 			\
	$(LOCAL_IFILES)


MOSTLYCLEANFILES = $(BUILT_SOURCES) *.pyc

# Don't distribute output of swig
dist-hook:
	@for file in $(BUILT_SOURCES); do echo $(RM) $(distdir)/$$file; done
	@for file in $(BUILT_SOURCES); do $(RM) $(distdir)/$$file; done

