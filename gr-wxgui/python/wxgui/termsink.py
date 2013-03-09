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

from gnuradio import gru
import wx

DEFAULT_WIN_SIZE = (600, 300)
APPEND_EVENT = wx.NewEventType()
EVT_APPEND_EVENT = wx.PyEventBinder(APPEND_EVENT, 0)

class AppendEvent(wx.PyEvent):
    def __init__(self, text):
        wx.PyEvent.__init__(self)
        self.SetEventType(APPEND_EVENT)
        self.text = text

    def Clone(self):
        self.__class__(self.GetId())

class termsink(wx.Panel):
	def __init__(self,
		     parent,
		     msgq,
		     size=DEFAULT_WIN_SIZE,
		     ):

		wx.Panel.__init__(self,
				  parent,
				  size=size,
				  style=wx.SIMPLE_BORDER,
				  )

		self.text_ctrl = wx.TextCtrl(self,
					     wx.ID_ANY,
					     value="",
					     size=size,
					     style=wx.TE_MULTILINE|wx.TE_READONLY,
					     )

		main_sizer = wx.BoxSizer(wx.VERTICAL)
		main_sizer.Add(self.text_ctrl, 1, wx.EXPAND)
		self.SetSizerAndFit(main_sizer)

                EVT_APPEND_EVENT(self, self.evt_append)
		self.runner = gru.msgq_runner(msgq, self.handle_msg)

	def handle_msg(self, msg):
		# This gets called in the queue runner thread context
		# For now, just add whatever the user sends to the text control
		text = msg.to_string()

		# Create a wxPython event and post it to the event queue
		evt = AppendEvent(text)
		wx.PostEvent(self, evt)
		del evt

        def evt_append(self, evt):
		# This gets called by the wxPython event queue runner
		self.text_ctrl.AppendText(evt.text)
