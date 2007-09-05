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
	$(USRP_INCLUDES) $(BOOST_CFLAGS) $(CPPUNIT_INCLUDES)


bin_PROGRAMS =				\
	usrper				\
	usrp_cal_dc_offset		

noinst_PROGRAMS =			\
	check_order_quickly		\
	test_usrp_inband_cs		\
	test_usrp_inband_ping		\
	test_usrp_inband_registers	\
	test_usrp_inband_rx		\
	test_usrp_inband_tx		\
	test_usrp_inband_timestamps	\
	test_usrp_standard_rx		\
	test_usrp_standard_tx		\
	read_packets

noinst_HEADERS =			\
	time_stuff.h			\
	ui_nco.h			\
	ui_sincos.h			

noinst_PYTHON =				\
	burn-db-eeprom			\
	burn-serial-number		


check_order_quickly_SOURCES	= check_order_quickly.cc

test_usrp_standard_rx_SOURCES 	= test_usrp_standard_rx.cc time_stuff.c
test_usrp_standard_rx_LDADD   	= $(USRP_LA)

test_usrp_standard_tx_SOURCES 	= test_usrp_standard_tx.cc time_stuff.c
test_usrp_standard_tx_LDADD   	= $(USRP_LA)

usrper_SOURCES			= usrper.cc
usrper_LDADD 			= $(USRP_LA)

usrp_cal_dc_offset_SOURCES	= usrp_cal_dc_offset.cc
usrp_cal_dc_offset_LDADD	= $(USRP_LA)

test_usrp_inband_cs_SOURCES 	= test_usrp_inband_cs.cc time_stuff.c ui_sincos.c
test_usrp_inband_cs_LDADD   	= $(USRP_LA)

test_usrp_inband_ping_SOURCES 	= test_usrp_inband_ping.cc time_stuff.c
test_usrp_inband_ping_LDADD   	= $(USRP_LA)

test_usrp_inband_tx_SOURCES 	= test_usrp_inband_tx.cc time_stuff.c ui_sincos.c
test_usrp_inband_tx_LDADD   	= $(USRP_LA)

test_usrp_inband_timestamps_SOURCES 	= test_usrp_inband_timestamps.cc time_stuff.c ui_sincos.c
test_usrp_inband_timestamps_LDADD   	= $(USRP_LA)

test_usrp_inband_registers_SOURCES 	= test_usrp_inband_registers.cc time_stuff.c ui_sincos.c
test_usrp_inband_registers_LDADD   	= $(USRP_LA)

test_usrp_inband_rx_SOURCES 	= test_usrp_inband_rx.cc time_stuff.c ui_sincos.c
test_usrp_inband_rx_LDADD   	= $(USRP_LA)

read_packets_SOURCES = read_packets.cc
read_packets_LDADD = $(USRP_LA)
