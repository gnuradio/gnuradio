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

import wx
import pubsub

DEFAULT_WIN_SIZE = (600, 300)

class term_window(wx.Panel, pubsub.pubsub):
	def __init__(self,
		     parent,
		     size,
		     ):

		pubsub.pubsub.__init__(self)
		wx.Panel.__init__(self,
				  parent,
				  size=size,
				  style=wx.SIMPLE_BORDER,
				  )

		self.text_ctrl = wx.TextCtrl(self,
					     wx.ID_ANY,
					     value="BOO",
					     size=size,
					     style=wx.TE_MULTILINE|wx.TE_READONLY,
					)

		main_sizer = wx.BoxSizer(wx.VERTICAL)
		main_sizer.Add(self.text_ctrl, 1, wx.EXPAND)
		self.SetSizerAndFit(main_sizer)
