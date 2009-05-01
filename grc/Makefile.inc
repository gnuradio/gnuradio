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
grc_gnuradio_prefix = $(pythondir)/grc_gnuradio
grc_src_prefix = $(pythondir)/gnuradio/grc
grc_data_prefix = $(datadir)/$(PACKAGE)/grc
grc_base_data_dir = $(grc_data_prefix)/platforms/base
grc_python_data_dir = $(grc_data_prefix)/platforms/python
grc_python_blocks_dir = $(grc_data_prefix)/platforms/python/blocks
