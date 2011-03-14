#
# Copyright 2011 Free Software Foundation, Inc.
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

usrp_pythondir = $(pythondir)/grc_gnuradio/usrp
usrp_python_PYTHON = \
	__init__.py \
	common.py \
	dual_usrp.py \
	simple_usrp.py

dist_bin_SCRIPTS = usrp_probe

grcblocksdir = $(grc_blocksdir)
dist_grcblocks_DATA = \
	usrp_dual_sink_x.xml \
	usrp_simple_sink_x.xml \
	usrp_dual_source_x.xml \
	usrp_simple_source_x.xml \
	usrp_probe.xml

ourdatadir = $(pkgdatadir)/grc/freedesktop
dist_ourdata_DATA = gnuradio-usrp_probe.desktop
