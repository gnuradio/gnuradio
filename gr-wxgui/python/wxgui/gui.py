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
from gnuradio import gr

#
# Top-level display panel with vertical box sizer.  User does not create or
# subclass this class; rather, the user supplies his own class constructor
# that gets invoked with needed parameters.
#
class top_panel(wx.Panel):
    def __init__(self, frame, top_block, gui, options, args):
        wx.Panel.__init__(self, frame, -1)
        vbox = wx.BoxSizer(wx.VERTICAL)

        # Create the user's GUI class
        if gui is not None:
            self.gui = gui(frame,          # Top-level window frame
                           self,           # Parent class for user created windows
                           vbox,           # Sizer for user to add windows to
                           top_block,      # GUI-unaware flowgraph to manipulate
                           options,        # Command-line options
                           args)           # Command-line arguments

        else:
            # User hasn't made their own GUI, create our default
            # We don't have a default GUI yet either :)
            p = wx.Panel(self)
            p.SetSize((640,480))
            vbox.Add(p, 1, wx.EXPAND)

        self.SetSizer(vbox)
        self.SetAutoLayout(True)
        vbox.Fit(self)

    def shutdown(self):
        try:
            self.gui.shutdown()
        except AttributeError:
            pass

#
# Top-level window frame with menu and status bars.
#
class top_frame(wx.Frame):
    def __init__ (self, top_block, gui, options, args,
                  title, nstatus, start, realtime):

        wx.Frame.__init__(self, None, -1, title)
        self.top_block = top_block

        self.CreateStatusBar(nstatus)
        mainmenu = wx.MenuBar()
        self.SetMenuBar(mainmenu)

        menu = wx.Menu()

        item = menu.Append(200, 'E&xit', 'Exit Application') # FIXME magic ID
        self.Bind(wx.EVT_MENU, self.OnCloseWindow, item)
        mainmenu.Append(menu, "&File")
        self.Bind(wx.EVT_CLOSE, self.OnCloseWindow)

        # Create main panel, creates user GUI class with supplied parameters
        self.panel = top_panel(self, top_block, gui, options, args)

        vbox = wx.BoxSizer(wx.VERTICAL)
        vbox.Add(self.panel, 1, wx.EXPAND)
        self.SetSizer(vbox)
        self.SetAutoLayout(True)
        vbox.Fit(self)

        if realtime:
            if gr.enable_realtime_scheduling() != gr.RT_OK:
                self.SetStatusText("Failed to enable realtime scheduling")

        if start and self.top_block is not None:
            self.top_block.start()

    def OnCloseWindow(self, event):
        # Give user API a chance to do something
        self.panel.shutdown()

        # Stop flowgraph as a convenience
        self.SetStatusText("Ensuring flowgraph has completed before exiting...")
        if self.top_block is not None:
            self.top_block.stop()
            self.top_block.wait()

        self.Destroy()


#
# Top-level wxPython application object.  User creates or subclasses this
# in their GUI script.
#
class app(wx.App):
    def __init__ (self, top_block=None, gui=None, options=None, args=None,
                  title="GNU Radio", nstatus=1, start=False, realtime=False):
        self.top_block = top_block
        self.gui = gui
        self.options = options
        self.args = args
        self.title = title
        self.nstatus = nstatus
        self.start = start
        self.realtime = realtime

        wx.App.__init__ (self, redirect=False)

    def OnInit(self):
        # Pass user parameters to top window frame
        frame = top_frame(self.top_block, self.gui, self.options, self.args,
                          self.title, self.nstatus, self.start, self.realtime)
        frame.Show(True)
        self.SetTopWindow(frame)
        return True
