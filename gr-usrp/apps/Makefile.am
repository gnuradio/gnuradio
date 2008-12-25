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
# You should have received a copy of the GNU General Public License
# along with GNU Radio; see the file COPYING.  If not, write to
# the Free Software Foundation, Inc., 51 Franklin Street,
# Boston, MA 02110-1301, USA.
# 

include $(top_srcdir)/Makefile.common

# For compiling within the GNU Radio build tree
AM_CPPFLAGS=$(STD_DEFINES_AND_INCLUDES) \
	 -I$(top_srcdir)/gr-usrp/src \
	 -I$(top_srcdir)/usrp/host/lib/legacy \
	 -I\${abs_top_builddir}/usrp/host/lib/legacy \
	 -I$(top_srcdir)/usrp/firmware/include \
	 $(WITH_INCLUDES)

GR_USRP_LA=$(top_builddir)/gr-usrp/src/libgnuradio-usrp.la

# For compiling outside the tree, these will get fished out by pkgconfig

noinst_PROGRAMS = \
	usrp_rx_cfile \
	usrp_siggen

noinst_HEADERS = 	\
	usrp_rx_cfile.h	\
	usrp_siggen.h

usrp_rx_cfile_SOURCES = 	\
    usrp_rx_cfile.cc

usrp_rx_cfile_LDADD = \
	$(BOOST_PROGRAM_OPTIONS_LIB) \
	$(GR_USRP_LA)

usrp_siggen_SOURCES = 	\
    usrp_siggen.cc

usrp_siggen_LDADD = \
	$(BOOST_PROGRAM_OPTIONS_LIB) \
	$(GR_USRP_LA)

MOSTLYCLEANFILES = *~
