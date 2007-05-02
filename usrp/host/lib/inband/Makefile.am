#
# Copyright 2007 Free Software Foundation, Inc.
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
# You should have received a copy of the GNU General Public License along
# with this program; if not, write to the Free Software Foundation, Inc.,
# 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
#

include $(top_srcdir)/Makefile.common

INCLUDES = 	\
	$(DEFINES) $(OMNITHREAD_INCLUDES) $(PMT_INCLUDES) $(MBLOCK_INCLUDES) \
	$(USRP_INCLUDES) $(BOOST_CFLAGS) $(CPPUNIT_INCLUDES)

TESTS = test_inband

EXTRA_DIST =				

lib_LTLIBRARIES = 			\
	libusrp_inband.la		\
	libusrp_inband-qa.la			


# ------------------------------------------------------------------------
# Build the inband library

BUILT_SOURCES =				\
	usrp_server_mbh.cc

usrp_server_mbh.cc : usrp_server.mbh
	$(COMPILE_MBH) usrp_server.mbh usrp_server_mbh.cc

libusrp_inband_la_SOURCES =		\
	$(BUILT_SOURCES)		\
	usrp_server.cc

libusrp_inband_la_LDFLAGS = $(NO_UNDEFINED) -version-info 0:0:0

libusrp_inband_la_LIBADD =		\
	$(MBLOCK_LA)			\
	-lstdc++


include_HEADERS =			\
	usrp_server.h			

noinst_HEADERS =      			\
	qa_inband.h			\
	qa_inband_packet_prims.h	\
	qa_inband_usrp_server.h

# ------------------------------------------------------------------------
# Build the qa code in its own library

libusrp_inband_qa_la_SOURCES =   	\
	qa_inband.cc			\
	qa_inband_packet_prims.cc	\
	qa_inband_usrp_server.cc	

# magic flags
libusrp_inband_qa_la_LDFLAGS = $(NO_UNDEFINED) -avoid-version

# link against c++ standard library
libusrp_inband_qa_la_LIBADD =    	\
	libusrp_inband.la		\
	$(CPPUNIT_LIBS)			\
	-lstdc++

# ------------------------------------------------------------------------

noinst_PROGRAMS =			\
	test_inband

test_inband_SOURCES = test_inband.cc
test_inband_LDADD   = libusrp_inband-qa.la


MOSTLYCLEANFILES = \
	$(BUILT_SOURCES) *~ *.pyc

