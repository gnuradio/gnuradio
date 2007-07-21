#!/usr/bin/env python
#
# Copyright 2005 Free Software Foundation, Inc.
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

import sys
import wx
from gnuradio.wxgui import form

class flexrf_debug_gui(wx.Frame):
    def __init__(self, flexrf, title="Flexrf Debug"):
        wx.Frame.__init__(self, None, -1, title)

        self.flexrf = flexrf
        
        self.CreateStatusBar (1)

        self.panel = wx.Panel(self, -1)
        self.vbox = wx.BoxSizer(wx.VERTICAL)
        self.panel.SetSizer(self.vbox)
        self.panel.SetAutoLayout(True)

        self._create_form()

        self.vbox.Fit(self.panel)

        self.frame_vbox = wx.BoxSizer(wx.VERTICAL)
        self.frame_vbox.Add(self.panel, 1, wx.EXPAND)
        self.SetSizer(self.frame_vbox)
        self.SetAutoLayout(True)
        self.frame_vbox.Fit(self)
        
    def _create_form(self):
        self._create_set_freq()
        self._create_write_fpga()
        self._create_write_all()
        self._create_write_it()
        #self._create_set_gain()
        
    # ----------------------------------------------------------------

    def _create_set_freq(self):

        def _set_freq(kv):
            return self.flexrf.set_freq(kv['freq'])[0]

        sbs = wx.StaticBoxSizer(wx.StaticBox(self.panel), wx.HORIZONTAL)
        sbs.Add((5,0), 0.1)       # stretchy space
        #sbs.Add(wx.StaticText(self.panel, -1, "set_freq "), 0, 0)
        #sbs.Add((5,0), 0.1)       # stretchy space
        myform = form.form()
        myform['freq'] = form.float_field(self.panel, sbs, "Set Frequency")
        sbs.Add((5,0), 0.1)       # stretchy space
        sbs.Add(form.button_with_callback(self.panel, "Do It!",
                                          self._generic_doit(_set_freq, myform)), 1, wx.EXPAND)
        sbs.Add((5,0), 0.1)       # stretchy space
        self.vbox.Add(sbs, 0, wx.EXPAND)


    def _create_write_fpga(self):

        def _write_fpga(kv):
            return self.flexrf._u._write_fpga_reg(kv['regno'], kv['value'])

        sbs = wx.StaticBoxSizer(wx.StaticBox(self.panel), wx.HORIZONTAL)
        sbs.Add((5,0), 0.1)       # stretchy space
        sbs.Add(wx.StaticText(self.panel, -1, "write_fpga_reg "), 0, 0)
        sbs.Add((5,0), 0.1)       # stretchy space
        myform = form.form()
        myform['regno'] = form.int_field(self.panel, sbs, "regno")
        sbs.Add((5,0), 0.1)       # stretchy space
        myform['value'] = form.int_field(self.panel, sbs, "value")
        sbs.Add((5,0), 0.1)       # stretchy space
        sbs.Add(form.button_with_callback(self.panel, "Do It!",
                                          self._generic_doit(_write_fpga, myform)), 1, wx.EXPAND)
        sbs.Add((5,0), 0.1)       # stretchy space
        self.vbox.Add(sbs, 0, wx.EXPAND)


    def _create_write_all(self):

        def _write_all(kv):
            self.flexrf._write_all(kv['R'], kv['control'], kv['N'])   # void
            return  True

        sbs = wx.StaticBoxSizer(wx.StaticBox(self.panel), wx.HORIZONTAL)
        sbs.Add((5,0), 0.1)       # stretchy space
        sbs.Add(wx.StaticText(self.panel, -1, "write_all "), 0, 0)
        sbs.Add((5,0), 0.1)       # stretchy space
        myform = form.form()
        myform['R'] = form.int_field(self.panel, sbs, "R")
        sbs.Add((5,0), 0.1)       # stretchy space
        myform['control'] = form.int_field(self.panel, sbs, "control")
        sbs.Add((5,0), 0.1)       # stretchy space
        myform['N'] = form.int_field(self.panel, sbs, "N")
        sbs.Add((5,0), 0.1)       # stretchy space
        sbs.Add(form.button_with_callback(self.panel, "Do It!",
                                          self._generic_doit(_write_all, myform)), 1, wx.EXPAND)
        sbs.Add((5,0), 0.1)       # stretchy space
        self.vbox.Add(sbs, 0, wx.EXPAND)


    def _create_write_it(self):

        def _write_it(kv):
            self.flexrf._write_it(kv['v'])      # void
            return True

        sbs = wx.StaticBoxSizer(wx.StaticBox(self.panel), wx.HORIZONTAL)
        sbs.Add((5,0), 0.1)       # stretchy space
        sbs.Add(wx.StaticText(self.panel, -1, "write_it "), 0, 0)
        sbs.Add((5,0), 0.1)       # stretchy space
        myform = form.form()
        myform['v'] = form.int_field(self.panel, sbs, "24-bit value")
        sbs.Add((5,0), 0.1)       # stretchy space
        sbs.Add(form.button_with_callback(self.panel, "Do It!",
                                          self._generic_doit(_write_it, myform)), 1, wx.EXPAND)
        sbs.Add((5,0), 0.1)       # stretchy space
        self.vbox.Add(sbs, 0, wx.EXPAND)


    # ----------------------------------------------------------------
    
    def _set_status_msg(self, msg):
        self.GetStatusBar().SetStatusText(msg, 0)

    def _generic_doit(self, callback, form):

        def button_callback():
            errors = form.check_input_for_errors()
            if errors:
                self._set_status_msg(errors[0])
                print '\n'.join(tuple(errors))
            else:
                kv = form.get_key_vals()
                if callback(kv):
                    self._set_status_msg("OK")
                else:
                    self._set_status_msg("Failed")

        return button_callback


        
if False and __name__ == '__main__':

    class demo_app (wx.App):
        def __init__ (self):
            wx.App.__init__(self)

        def OnInit (self):
            frame = flexrf_debug_gui(None, "Debug FlexRF TX")
            frame.Show(True)
            self.SetTopWindow (frame)
            return True

    app = demo_app()
    app.MainLoop()

