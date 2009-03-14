#
# Copyright 2007,2009 Free Software Foundation, Inc.
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

RBFS = usrp_sounder.rbf

rbf2datadir = $(prefix)/share/usrp/rev2
dist_rbf2data_DATA = $(RBFS)

rbf4datadir = $(prefix)/share/usrp/rev4
dist_rbf4data_DATA = $(RBFS)

EXTRA_DIST =			\
	config.vh		\
	usrp_sounder.v		\
	usrp_sounder.csf	\
	usrp_sounder.esf	\
	usrp_sounder.psf	\
	usrp_sounder.qpf

MOSTLYCLEANFILES += 	\
	db/*		\
	*.rpt		\
	*.summary	\
	*.qws		\
	*.smsg		\
	*.done		\
	*.pin		\
	*.sof
