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
	$(USRP_INCLUDES) $(USRP_INBAND_INCLUDES) $(BOOST_CFLAGS) \
	$(CPPUNIT_INCLUDES) $(WITH_INCLUDES)


bin_PROGRAMS =				

noinst_PROGRAMS =			\
	test_usrp_inband_cs		\
	test_usrp_inband_ping		\
	test_usrp_inband_registers	\
	test_usrp_inband_rx		\
	test_usrp_inband_tx		\
	test_usrp_inband_timestamps	\
	test_usrp_inband_overrun	\
	test_usrp_inband_underrun	\
	test_gmac_tx			\
	read_packets

noinst_HEADERS =			\
	ui_nco.h			\
	ui_sincos.h			\
	gmac.h				\
	gmac_symbols.h			


EXTRA_DIST =				\
	gmac.mbh

#------------------------------------------------------------------
# Build gmac sources

BUILT_SOURCES =				\
	gmac_mbh.cc

gmac_mbh.cc : gmac.mbh
	$(COMPILE_MBH) $(srcdir)/gmac.mbh gmac_mbh.cc


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

test_usrp_inband_overrun_SOURCES 	= test_usrp_inband_overrun.cc
test_usrp_inband_overrun_LDADD   	= $(USRP_LA) $(USRP_INBAND_LA)

test_usrp_inband_underrun_SOURCES 	= test_usrp_inband_underrun.cc
test_usrp_inband_underrun_LDADD   	= $(USRP_LA) $(USRP_INBAND_LA)

test_usrp_inband_rx_SOURCES 	= test_usrp_inband_rx.cc ui_sincos.c
test_usrp_inband_rx_LDADD   	= $(USRP_LA) $(USRP_INBAND_LA)

test_gmac_tx_SOURCES 	= test_gmac_tx.cc gmac.cc gmac_mbh.cc ui_sincos.c
test_gmac_tx_LDADD   	= $(USRP_LA) $(USRP_INBAND_LA)

read_packets_SOURCES = read_packets.cc
read_packets_LDADD = $(USRP_LA) $(USRP_INBAND_LA)
