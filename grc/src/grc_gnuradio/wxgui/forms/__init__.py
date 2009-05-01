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

"""
The following classes will be available through gnuradio.wxgui.forms:
""" 

########################################################################
# External Converters
########################################################################
from converters import \
	eval_converter, str_converter, \
	float_converter, int_converter

########################################################################
# External Forms
########################################################################
from forms import \
	radio_buttons, drop_down, notebook, \
	button, toggle_button, single_button, \
	check_box, text_box, static_text, \
	slider, log_slider

########################################################################
# Helpful widgets
########################################################################
import wx

class static_box_sizer(wx.StaticBoxSizer):
	def __init__(self, parent, label='', bold=False, orient=wx.VERTICAL):
		box = wx.StaticBox(parent=parent, label=label)
		if bold:
			font = box.GetFont()
			font.SetWeight(wx.FONTWEIGHT_BOLD)
			box.SetFont(font)
		wx.StaticBoxSizer.__init__(self, box=box, orient=orient)
