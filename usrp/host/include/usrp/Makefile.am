#
# Copyright 2009 Free Software Foundation, Inc.
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

usrpincludedir = $(includedir)/usrp

usrpinclude_HEADERS = \
	db_base.h \
	db_basic.h \
	db_bitshark_rx.h \
	db_dbs_rx.h \
	db_dtt754.h \
	db_dtt768.h \
	db_flexrf.h \
	db_flexrf_mimo.h \
	db_tv_rx.h \
	db_tv_rx_mimo.h	\
	db_wbxng.h \
	db_xcvr2450.h \
	libusb_types.h \
	usrp_basic.h \
	usrp_bytesex.h \
	usrp_dbid.h \
	usrp_local_sighandler.h \
	usrp_prims.h \
	usrp_slots.h \
	usrp_standard.h \
	usrp_subdev_spec.h \
	usrp_tune_result.h

EXTRA_DIST = \
	libusb_types.h.in

if PYTHON
swiginclude_HEADERS = \
	db_base.i
endif
