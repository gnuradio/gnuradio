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

# Install this stuff so that it ends up as the gnuradio.usrp module
# This usually ends up at:
#   ${prefix}/lib/python${python_version}/site-packages/gnuradio

ourpythondir = $(grpythondir)
ourlibdir    = $(grpyexecdir)

EXTRA_DIST = run_tests.in
TESTS = run_tests

LOCAL_IFILES = 				\
	$(top_srcdir)/gr-usrp/src/usrp1.i				

NON_LOCAL_IFILES = $(GNURADIO_I)

ALL_IFILES = 				\
	$(LOCAL_IFILES)			\
	$(NON_LOCAL_IFILES)		

BUILT_SOURCES = 			\
	usrp1.cc			\
	usrp1.py			

ourpython_PYTHON =			\
	db_base.py			\
	db_basic.py			\
	db_dbs_rx.py			\
	db_flexrf.py			\
	db_flexrf_mimo.py		\
	db_wbx.py			\
	db_xcvr2450.py			\
	db_instantiator.py		\
	db_tv_rx.py			\
	flexrf_debug_gui.py		\
	tx_debug_gui.py			\
	usrp.py				\
	usrp1.py			\
        usrp_multi.py			


AM_CPPFLAGS = $(STD_DEFINES_AND_INCLUDES)  \
	   $(PYTHON_CPPFLAGS) 		\
	   $(USRP_INCLUDES)		\
	   $(WITH_INCLUDES)

SWIGPYTHONARGS = $(SWIGPYTHONFLAGS)	     \
		 $(STD_DEFINES_AND_INCLUDES) \
		 $(USRP_INCLUDES)	     \
		 $(WITH_INCLUDES)            \
		 $(WITH_SWIG_INCLUDES)

grinclude_HEADERS =			\
	usrp1_sink_base.h		\
	usrp1_sink_c.h			\
	usrp1_sink_s.h			\
	usrp1_source_base.h		\
	usrp1_source_c.h		\
	usrp1_source_s.h		

swiginclude_HEADERS = 			\
	$(LOCAL_IFILES)


ourlib_LTLIBRARIES = _usrp1.la


_usrp1_la_SOURCES = 			\
	usrp1.cc			\
	usrp1_sink_base.cc		\
	usrp1_sink_c.cc			\
	usrp1_sink_s.cc			\
	usrp1_source_base.cc		\
	usrp1_source_c.cc		\
	usrp1_source_s.cc		


_usrp1_la_LIBADD = 			\
	$(PYTHON_LDFLAGS)		\
	$(GNURADIO_CORE_LA)		\
	$(USRP_LA)			\
	-lstdc++


_usrp1_la_LDFLAGS = $(NO_UNDEFINED) -module -avoid-version


usrp1.cc usrp1.py: usrp1.i $(NON_LOCAL_IFILES) $(LOCAL_IFILES)
	$(SWIG) $(SWIGPYTHONARGS) -module usrp1 -o usrp1.cc $(LOCAL_IFILES)


noinst_PYTHON =				\
	qa_usrp.py			

MOSTLYCLEANFILES = \
	$(BUILT_SOURCES) *~ *.pyc


# Don't distribute output of swig
dist-hook:
	@for file in $(BUILT_SOURCES); do echo $(RM) $(distdir)/$$file; done
	@for file in $(BUILT_SOURCES); do $(RM) $(distdir)/$$file; done
