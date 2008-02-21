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

AM_CPPFLAGS = $(USRP_INCLUDES) $(BOOST_CFLAGS) $(CPPUNIT_INCLUDES) \
	   $(WITH_INCLUDES)

bin_PROGRAMS =				\
	usrper				\
	usrp_cal_dc_offset		

noinst_PROGRAMS =			\
	check_order_quickly		\
	test_usrp_standard_rx		\
	test_usrp_standard_tx		

noinst_HEADERS =			\
	time_stuff.h			

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
