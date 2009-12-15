# Copyright 2008, 2009 Free Software Foundation, Inc.
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
from gnuradio import gr
import panel

default_gui_size = (200, 100)

class top_block_gui(gr.top_block):
	"""gr top block with wx gui app and grid sizer."""

	def __init__(self, title='', size=default_gui_size):
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
		#create gui elements
		self._app = wx.App()
		self._frame = wx.Frame(None, title=title)
		self._panel = panel.Panel(self._frame)
		self.Add = self._panel.Add
		self.GridAdd = self._panel.GridAdd
		self.GetWin = self._panel.GetWin

	def SetIcon(self, *args, **kwargs): self._frame.SetIcon(*args, **kwargs)

	def Run(self, start=True):
		"""
		Setup the wx gui elements.
		Start the gr top block.
		Block with the wx main loop.
		"""
		#set minimal window size
		self._frame.SetSizeHints(*self._size)
		#create callback for quit
		def _quit(event):
			self.stop(); self.wait()
			self._frame.Destroy()
		#setup app
		self._frame.Bind(wx.EVT_CLOSE, _quit)
		self._sizer = wx.BoxSizer(wx.VERTICAL)
		self._sizer.Add(self._panel, 0, wx.EXPAND)
		self._frame.SetSizerAndFit(self._sizer)
		self._frame.SetAutoLayout(True)
		self._frame.Show(True)
		self._app.SetTopWindow(self._frame)
		#start flow graph
		if start: self.start()
		#blocking main loop
		self._app.MainLoop()
