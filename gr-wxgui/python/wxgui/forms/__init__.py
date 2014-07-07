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
	slider, log_slider, gauge, \
	make_bold, DataEvent, EVT_DATA

########################################################################
# Helpful widgets
########################################################################
import wx

class static_box_sizer(wx.StaticBoxSizer):
	"""
	A box sizer with label and border.

        Args:
	    parent: the parent widget
            sizer: add this widget to sizer if provided (optional)
	    proportion: the proportion when added to the sizer (default=0)
	    flag: the flag argument when added to the sizer (default=wx.EXPAND)
	    label: title label for this widget (optional)
	    bold: true to boldify the label
	    orient: the sizer orientation wx.VERTICAL or wx.HORIZONTAL (default=wx.VERTICAL)
	"""
	def __init__(self, parent, label='', bold=False, sizer=None, orient=wx.VERTICAL, proportion=0, flag=wx.EXPAND):
		box = wx.StaticBox(parent=parent, label=label)
		if bold: make_bold(box)
		wx.StaticBoxSizer.__init__(self, box=box, orient=orient)
		if sizer: sizer.Add(self, proportion, flag)

class incr_decr_buttons(wx.BoxSizer):
	"""
	A horizontal box sizer with a increment and a decrement button.

        Args:
	    parent: the parent widget
	    on_incr: the callback for pressing the + button
	    on_decr: the callback for pressing the - button
	    label: title label for this widget (optional)
	    sizer: add this widget to sizer if provided (optional)
	    proportion: the proportion when added to the sizer (default=0)
	    flag: the flag argument when added to the sizer (default=wx.EXPAND)
	"""
	def __init__(self, parent, on_incr, on_decr, label='', sizer=None, proportion=0, flag=wx.EXPAND):
		wx.BoxSizer.__init__(self, wx.HORIZONTAL)
		buttons_box = wx.BoxSizer(wx.HORIZONTAL)
		self._incr_button = wx.Button(parent, label='+', style=wx.BU_EXACTFIT)
		self._incr_button.Bind(wx.EVT_BUTTON, on_incr)
		buttons_box.Add(self._incr_button, 0, wx.ALIGN_CENTER_VERTICAL)
		self._decr_button = wx.Button(parent, label=' - ', style=wx.BU_EXACTFIT)
		self._decr_button.Bind(wx.EVT_BUTTON, on_decr)
		buttons_box.Add(self._decr_button, 0, wx.ALIGN_CENTER_VERTICAL)
		if label: #add label
			self.Add(wx.StaticText(parent, label='%s: '%label), 1, wx.ALIGN_CENTER_VERTICAL | wx.ALIGN_LEFT)
			self.Add(buttons_box, 0, wx.ALIGN_CENTER_VERTICAL | wx.ALIGN_RIGHT)
		else: self.Add(buttons_box, 0, wx.ALIGN_CENTER_VERTICAL)
		if sizer: sizer.Add(self, proportion, flag)

	def Disable(self, disable=True): self.Enable(not disable)
	def Enable(self, enable=True):
		if enable:
			self._incr_button.Enable()
			self._decr_button.Enable()
		else:
			self._incr_button.Disable()
			self._decr_button.Disable()
