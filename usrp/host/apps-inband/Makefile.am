#
# Copyright 2003,2006,2008 Free Software Foundation, Inc.
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

AM_CPPFLAGS = 	\
	$(DEFINES) $(OMNITHREAD_INCLUDES) $(PMT_INCLUDES) $(MBLOCK_INCLUDES) \
	$(USRP_INCLUDES) $(USRP_INBAND_INCLUDES) $(BOOST_CPPFLAGS) \
	$(CPPUNIT_INCLUDES) $(WITH_INCLUDES) -I$(top_srcdir)/mblock/src/lib


bin_PROGRAMS =				

noinst_PROGRAMS =			\
	test_usrp_inband_ping		\
	test_usrp_inband_registers	\
	test_usrp_inband_rx		\
	test_usrp_inband_2rx		\
	test_usrp_inband_tx		\
	test_usrp_inband_2tx		\
	test_usrp_inband_timestamps	\
	test_usrp_inband_overrun	\
	test_usrp_inband_underrun	\
	read_packets

noinst_HEADERS =			\
	ui_nco.h			\
	ui_sincos.h	


test_usrp_inband_ping_SOURCES 	= test_usrp_inband_ping.cc 
test_usrp_inband_ping_LDADD   	= $(USRP_LA) $(USRP_INBAND_LA)

test_usrp_inband_tx_SOURCES 	= test_usrp_inband_tx.cc ui_sincos.c
test_usrp_inband_tx_LDADD   	= $(USRP_LA) $(USRP_INBAND_LA)

test_usrp_inband_2tx_SOURCES 	= test_usrp_inband_2tx.cc ui_sincos.c
test_usrp_inband_2tx_LDADD   	= $(USRP_LA) $(USRP_INBAND_LA)

test_usrp_inband_timestamps_SOURCES 	= test_usrp_inband_timestamps.cc ui_sincos.c
test_usrp_inband_timestamps_LDADD   	= $(USRP_LA) $(USRP_INBAND_LA)

test_usrp_inband_registers_SOURCES 	= test_usrp_inband_registers.cc ui_sincos.c
test_usrp_inband_registers_LDADD   	= $(USRP_LA) $(USRP_INBAND_LA)

test_usrp_inband_overrun_SOURCES 	= test_usrp_inband_overrun.cc
test_usrp_inband_overrun_LDADD   	= $(USRP_LA) $(USRP_INBAND_LA)

test_usrp_inband_underrun_SOURCES 	= test_usrp_inband_underrun.cc
test_usrp_inband_underrun_LDADD   	= $(USRP_LA) $(USRP_INBAND_LA)

test_usrp_inband_rx_SOURCES 	= test_usrp_inband_rx.cc ui_sincos.c
test_usrp_inband_rx_LDADD   	= $(USRP_LA) $(USRP_INBAND_LA)

test_usrp_inband_2rx_SOURCES 	= test_usrp_inband_2rx.cc ui_sincos.c
test_usrp_inband_2rx_LDADD   	= $(USRP_LA) $(USRP_INBAND_LA)

read_packets_SOURCES = read_packets.cc
read_packets_LDADD = $(USRP_LA) $(USRP_INBAND_LA)
