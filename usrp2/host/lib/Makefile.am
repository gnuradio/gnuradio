#
# Copyright 2007,2008,2010 Free Software Foundation, Inc.
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.
#

include $(top_srcdir)/Makefile.common

AM_CPPFLAGS = \
    $(USRP2_INCLUDES) \
    $(GRUEL_INCLUDES) \
    $(STD_DEFINES_AND_INCLUDES) \
    $(BOOST_CPPFLAGS) \
    $(CPPUNIT_INCLUDES)

bin_PROGRAMS = usrp2_socket_opener
usrp2_socket_opener_SOURCES = usrp2_socket_opener.cc

lib_LTLIBRARIES = \
	libusrp2.la

libusrp2_la_LDFLAGS = $(LTVERSIONFLAGS)

libusrp2_la_SOURCES = \
	control.cc \
	copiers.cc \
	copy_handler.cc \
	data_handler.cc \
	eth_buffer.cc \
	ethernet.cc \
	find.cc \
	open_usrp2_socket.cc \
	pktfilter.cc \
	ring.cc \
	rx_nop_handler.cc \
	rx_sample_handler.cc \
	strtod_si.c \
	usrp2.cc \
	usrp2_impl.cc

libusrp2_la_LIBADD = \
	$(GRUEL_LA) \
	$(BOOST_LDFLAGS) $(BOOST_THREAD_LIB)

# Private headers not needed for above the API development
noinst_HEADERS = \
	control.h \
	eth_buffer.h \
	eth_common.h \
	ethernet.h \
        open_usrp2_socket.h \
	pktfilter.h \
	ring.h \
	usrp2_bytesex.h \
	usrp2_impl.h