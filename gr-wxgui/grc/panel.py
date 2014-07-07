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

class Panel(wx.Panel):
	def __init__(self, parent, orient=wx.VERTICAL):
		wx.Panel.__init__(self, parent)
		self._box = wx.BoxSizer(orient)
		self._grid = wx.GridBagSizer(5, 5)
		self.Add(self._grid)
		self.SetSizer(self._box)

	def GetWin(self): return self

	def Add(self, win):
		"""
		Add a window to the wx vbox.

		Args:
		    win: the wx window
		"""
		self._box.Add(win, 0, wx.EXPAND)

	def GridAdd(self, win, row, col, row_span=1, col_span=1):
		"""
		Add a window to the wx grid at the given position.

		Args:
		    win: the wx window
		    row: the row specification (integer >= 0)
		    col: the column specification (integer >= 0)
		    row_span: the row span specification (integer >= 1)
		    col_span: the column span specification (integer >= 1)
		"""
		self._grid.Add(win, wx.GBPosition(row, col), wx.GBSpan(row_span, col_span), wx.EXPAND)
