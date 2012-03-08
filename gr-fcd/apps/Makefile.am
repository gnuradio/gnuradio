#
# Copyright 2011 Free Software Foundation, Inc.
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

# For compiling within the build tree
#AM_CPPFLAGS = \
#	-I$(top_srcdir)/lib/ \
#	$(AM_CPPFLAGS) \
#	$(STD_DEFINES_AND_INCLUDES) \
#	$(WITH_INCLUDES)

GR_FCD_LA=$(top_builddir)/lib/libgnuradio-fcd.la

# For compiling outside the tree, these will get fished out by pkgconfig
LDADD = \
	$(GR_FCD_LA) \
	$(GNURADIO_CORE_LA) \
    $(GNURADIO_AUDIO_LA) \
    $(BOOST_PROGRAM_OPTIONS_LIB)

bin_PROGRAMS = \
	fcd_nfm_rx

fcd_nfm_rx_SOURCES = \
	fcd_nfm_rx.cc


if PYTHON
# FIXME
#dist_bin_SCRIPTS = \
#    fcd_nfm_rx.py

EXTRA_DIST = \
    fcd_fft_wx.grc \
    fcd_apt_rx.grc \
    fcd_nfm_rx.grc

endif
