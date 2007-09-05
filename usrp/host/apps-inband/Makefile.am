#
# Copyright 2003,2006 Free Software Foundation, Inc.
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

INCLUDES = 	\
	$(DEFINES) $(OMNITHREAD_INCLUDES) $(PMT_INCLUDES) $(MBLOCK_INCLUDES) \
	$(USRP_INCLUDES) $(USRP_INBAND_INCLUDES) $(BOOST_CFLAGS) $(CPPUNIT_INCLUDES)


bin_PROGRAMS =				

noinst_PROGRAMS =			\
	test_usrp_inband_cs		\
	test_usrp_inband_ping		\
	test_usrp_inband_registers	\
	test_usrp_inband_rx		\
	test_usrp_inband_tx		\
	test_usrp_inband_timestamps	\
	read_packets

noinst_HEADERS =			\
	ui_nco.h			\
	ui_sincos.h			


test_usrp_inband_cs_SOURCES 	= test_usrp_inband_cs.cc ui_sincos.c
test_usrp_inband_cs_LDADD   	= $(USRP_LA) $(USRP_INBAND_LA)

test_usrp_inband_ping_SOURCES 	= test_usrp_inband_ping.cc 
test_usrp_inband_ping_LDADD   	= $(USRP_LA) $(USRP_INBAND_LA)

test_usrp_inband_tx_SOURCES 	= test_usrp_inband_tx.cc ui_sincos.c
test_usrp_inband_tx_LDADD   	= $(USRP_LA) $(USRP_INBAND_LA)

test_usrp_inband_timestamps_SOURCES 	= test_usrp_inband_timestamps.cc ui_sincos.c
test_usrp_inband_timestamps_LDADD   	= $(USRP_LA) $(USRP_INBAND_LA)

test_usrp_inband_registers_SOURCES 	= test_usrp_inband_registers.cc ui_sincos.c
test_usrp_inband_registers_LDADD   	= $(USRP_LA) $(USRP_INBAND_LA)

test_usrp_inband_rx_SOURCES 	= test_usrp_inband_rx.cc ui_sincos.c
test_usrp_inband_rx_LDADD   	= $(USRP_LA) $(USRP_INBAND_LA)

read_packets_SOURCES = read_packets.cc
read_packets_LDADD = $(USRP_LA) $(USRP_INBAND_LA)
