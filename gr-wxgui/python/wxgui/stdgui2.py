#
# Copyright 2004 Free Software Foundation, Inc.
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

'''A simple wx gui for GNU Radio applications'''

import ctypes
import wx
import sys
from gnuradio import gr


class stdapp (wx.App):
    def __init__ (self, top_block_maker, title="GNU Radio", nstatus=2,
                  max_noutput_items=None):
        self.top_block_maker = top_block_maker
        self.title = title
        self._nstatus = nstatus
        self._max_noutput_items = max_noutput_items
        # If we're on Linux, also enable multi-threading Xlib access
        if sys.platform.startswith('linux'):
            try:
                x11 = ctypes.cdll.LoadLibrary('libX11.so')
                x11.XInitThreads()
            except:
                print "Warning: failed to XInitThreads()"
        # All our initialization must come before calling wx.App.__init__.
        # OnInit is called from somewhere in the guts of __init__.
        wx.App.__init__ (self, redirect=False)

    def OnInit (self):
        frame = stdframe (self.top_block_maker, self.title, self._nstatus)
        frame.Show (True)
        self.SetTopWindow (frame)

        if(self._max_noutput_items is not None):
            frame.top_block().start (self._max_noutput_items)
        else:
            frame.top_block().start ()

        return True


class stdframe (wx.Frame):
    def __init__ (self, top_block_maker, title="GNU Radio", nstatus=2):
        # print "stdframe.__init__"
        wx.Frame.__init__(self, None, -1, title)

        self.CreateStatusBar (nstatus)
        mainmenu = wx.MenuBar ()

        menu = wx.Menu ()
        item = menu.Append (200, 'E&xit', 'Exit')
        self.Bind (wx.EVT_MENU, self.OnCloseWindow, item)
        mainmenu.Append (menu, "&File")
        self.SetMenuBar (mainmenu)

        self.Bind (wx.EVT_CLOSE, self.OnCloseWindow)
        self.panel = stdpanel (self, self, top_block_maker)
        vbox = wx.BoxSizer(wx.VERTICAL)
        vbox.Add(self.panel, 1, wx.EXPAND)
        self.SetSizer(vbox)
        self.SetAutoLayout(True)
        vbox.Fit(self)

    def OnCloseWindow (self, event):
        self.top_block().stop()
        self.top_block().wait()
        self.Destroy ()

    def top_block (self):
        return self.panel.top_block

class stdpanel (wx.Panel):
    def __init__ (self, parent, frame, top_block_maker):
        # print "stdpanel.__init__"
        wx.Panel.__init__ (self, parent, -1)
        self.frame = frame

        vbox = wx.BoxSizer (wx.VERTICAL)
        self.top_block = top_block_maker (frame, self, vbox, sys.argv)
        self.SetSizer (vbox)
        self.SetAutoLayout (True)
        vbox.Fit (self)

class std_top_block (gr.top_block):
    def __init__ (self, parent, panel, vbox, argv):
        # Call the hier_block2 constructor
        # Top blocks have no inputs and outputs
        gr.top_block.__init__(self, "std_top_block")
