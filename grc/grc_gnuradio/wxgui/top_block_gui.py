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

import wx
import sys, os
from gnuradio import gr

default_gui_size = (200, 100)

class top_block_gui(gr.top_block):
	"""gr top block with wx gui app and grid sizer."""

	def __init__(self, title='', size=default_gui_size, icon=None):
		"""
		Initialize the gr top block.
		Create the wx gui elements.
		@param title the main window title
		@param size the main window size tuple in pixels
		@param icon the file path to an icon or None
		"""
		#initialize
		gr.top_block.__init__(self)
		self._size = size
		#set the icon
		if icon and os.path.isfile(icon): self._icon = icon
		else: self._icon = None
		#create gui elements
		self._wx_app = wx.App()
		self._wx_frame = wx.Frame(None , -1, title)
		self._wx_grid = wx.GridBagSizer(5, 5)
		self._wx_vbox = wx.BoxSizer(wx.VERTICAL)

	def GetWin(self):
		"""
		Get the window for wx elements to fit within.
		@return the wx frame
		"""
		return self._wx_frame

	def Add(self, win):
		"""
		Add a window to the wx vbox.
		@param win the wx window
		"""
		self._wx_vbox.Add(win, 0, wx.EXPAND)

	def GridAdd(self, win, row, col, row_span=1, col_span=1):
		"""
		Add a window to the wx grid at the given position.
		@param win the wx window
		@param row the row specification (integer >= 0)
		@param col the column specification (integer >= 0)
		@param row_span the row span specification (integer >= 1)
		@param col_span the column span specification (integer >= 1)
		"""
		self._wx_grid.Add(win, wx.GBPosition(row, col), wx.GBSpan(row_span, col_span), wx.EXPAND)

	def start(self, start=True):
		if start:
			gr.top_block.start(self)
		else:
			gr.top_block.stop(self)
			gr.top_block.wait(self)

	def Run(self, autostart=True):
		"""
		Setup the wx gui elements.
		Start the gr top block.
		Block with the wx main loop.
		"""
		#set wx app icon
		if self._icon: self._wx_frame.SetIcon(wx.Icon(self._icon, wx.BITMAP_TYPE_ANY))
		#set minimal window size
		self._wx_frame.SetSizeHints(*self._size)
		#create callback for quit
		def _quit(event):
			gr.top_block.stop(self)
			self._wx_frame.Destroy()
		#setup app
		self._wx_vbox.Add(self._wx_grid, 0, wx.EXPAND)
		self._wx_frame.Bind(wx.EVT_CLOSE, _quit)
		self._wx_frame.SetSizerAndFit(self._wx_vbox)
		self._wx_frame.Show()
		self._wx_app.SetTopWindow(self._wx_frame)
		#start flow graph
		self.start(autostart)
		#blocking main loop
		self._wx_app.MainLoop()
