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
    $(CPPUNIT_INCLUDES)

LDADD = \
    $(USRP2_LA) \
    $(GRUEL_LA)

bin_PROGRAMS = \
	find_usrps \
	usrp2_burn_mac_addr

noinst_PROGRAMS = \
	gen_const \
	rx_streaming_samples \
	tx_samples \
	test_mimo_tx \
	gpio

find_usrps_SOURCES = find_usrps.cc
usrp2_burn_mac_addr_SOURCES = usrp2_burn_mac_addr.cc
rx_streaming_samples_SOURCES = rx_streaming_samples.cc
gen_const_SOURCES = gen_const.cc
tx_samples_SOURCES = tx_samples.cc
test_mimo_tx_SOURCES = test_mimo_tx.cc
gpio_SOURCES = gpio.cc
