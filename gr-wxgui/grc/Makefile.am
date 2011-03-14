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

grcblocksdir = $(grc_blocksdir)
dist_grcblocks_DATA = \
	notebook.xml \
	variable_check_box.xml \
	variable_chooser.xml \
	variable_slider.xml \
	variable_static_text.xml \
	variable_text_box.xml \
	wxgui_constellationsink2.xml \
	wxgui_fftsink2.xml \
	wxgui_histosink2.xml \
	wxgui_numbersink2.xml \
	wxgui_scopesink2.xml \
	wxgui_termsink.xml \
	wxgui_waterfallsink2.xml

#The wxgui module contains a top_block + wxgui frame.
wxgui_pythondir = $(pythondir)/grc_gnuradio/wxgui
wxgui_python_PYTHON = \
	__init__.py \
	panel.py \
	top_block_gui.py
