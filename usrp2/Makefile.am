#
# Copyright 2008 Free Software Foundation, Inc.
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
# You should have received a copy of the GNU General Public License along
# with this program; if not, write to the Free Software Foundation, Inc.,
# 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
#

include $(top_srcdir)/Makefile.common

SUBDIRS = host

# KLUDGE ALERT!
#
# This kludge gets the files the host code needs into the tarball even if
# we're not building the firmware.
#
if !BUILDING_USRP2_FIRMWARE
EXTRA_DIST = \
    firmware/include/usrp2_cdefs.h \
    firmware/include/usrp2_cdefs.h \
    firmware/include/usrp2_eth_packet.h \
    firmware/include/usrp2_fpga_regs.h \
    firmware/include/usrp2_i2c_addr.h \
    firmware/include/usrp2_mac_addr.h \
    firmware/include/usrp2_mimo_config.h \
    firmware/include/usrp2_types.h
endif
