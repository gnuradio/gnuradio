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

class tx_debug_gui(wx.Frame):
    def __init__(self, tx_subdev, title="Tx Debug"):
        wx.Frame.__init__(self, None, -1, title)

        self.subdev = tx_subdev
        self.subdev._u.set_verbose(True)
        
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
        
    # ----------------------------------------------------------------

    def _write_9862(self, regno, v):
        return self.subdev._u._write_9862(self.subdev._which, regno, v)

    def _set_dac_offset(self, i_or_q, offset, offset_pin):
        return self.subdev._u.set_dac_offset(self.subdev._which * 2 + i_or_q, offset, offset_pin)

    def _set_dac_fine_gain(self, i_or_q, gain, coarse):
        return self._write_9862(14 + i_or_q, (coarse & 0xC0) | (gain & 0x3f))

    def _create_form(self):
        self._create_dac_offset()
        self._create_dac_fine_gain()
        self._create_pga()
        
    # ----------------------------------------------------------------

    def _create_dac_offset(self):

        sbs = wx.StaticBoxSizer(wx.StaticBox(self.panel), wx.VERTICAL)

        hbox = wx.BoxSizer(wx.HORIZONTAL)
        hbox.Add(wx.StaticText(self.panel, -1, "DAC Offset"), 5, 0)
        sbs.Add(hbox, 0, 1)


        self._create_dac_offset_helper(sbs, 0)
        self._create_dac_offset_helper(sbs, 1)

        self.vbox.Add(sbs, 0, wx.EXPAND)

    def _create_dac_offset_helper(self, vbox, i_or_q):

        def doit(kv):
            drive_positive = kv['drive_positive']
            dac_offset = kv['dac_offset']
            print "drive_positive =", drive_positive
            print "dac_offset[%d] = %4d" % (i_or_q, dac_offset)
            
            # FIXME signed magnitude??
            # dac_offset = signed_mag10(dac_offset)
            return self._set_dac_offset(i_or_q, dac_offset, int(drive_positive))
        
        def signed_mag10(x):
            # not clear from doc if this is really 2's comp or 10-bit signed magnitude
            # we'll guess it's 10-bit signed mag
            if x < 0:
                return (1 << 9) | min(511, max(0, abs(x)))
            else:
                return (0 << 9) | min(511, max(0, abs(x)))

        myform = form.form()
        hbox = wx.BoxSizer(wx.HORIZONTAL)
        vbox.Add(hbox, 0, wx.EXPAND)
        myform['drive_positive'] = form.checkbox_field(parent=self.panel, sizer=hbox,
                                                       callback=myform.check_input_and_call(doit),
                                                       weight=0,
                                                       label="drive +ve")
        myform['dac_offset'] = form.slider_field(parent=self.panel, sizer=hbox,
                                                 callback=myform.check_input_and_call(doit),
                                                 min=-512, max=511, value=0,
                                                 weight=5)
        
    # ----------------------------------------------------------------

    def _create_dac_fine_gain(self):
        sbs = wx.StaticBoxSizer(wx.StaticBox(self.panel), wx.VERTICAL)

        hbox = wx.BoxSizer(wx.HORIZONTAL)
        hbox.Add(wx.StaticText(self.panel, -1, "DAC Gain"), 5, 0)
        sbs.Add(hbox, 0, 1)

        self._create_dac_gain_helper(sbs, 0)
        self._create_dac_gain_helper(sbs, 1)

        self.vbox.Add(sbs, 0, wx.EXPAND)

    def _create_dac_gain_helper(self, vbox, i_or_q):

        d = { "1/1"  : 0xC0,
              "1/2"  : 0x40,
              "1/11" : 0x00 }

        def doit(kv):
            dac_gain = kv['dac_gain']
            coarse_s = kv['coarse']
            print "dac_gain[%d] = %4d" % (i_or_q, dac_gain)
            print "coarse = ", coarse_s
            return self._set_dac_fine_gain(i_or_q, dac_gain, d[coarse_s])
        
        myform = form.form()
        hbox = wx.BoxSizer(wx.HORIZONTAL)
        vbox.Add(hbox, 0, wx.EXPAND)
        myform['coarse'] = form.radiobox_field(parent=self.panel, sizer=hbox,
                                               callback=myform.check_input_and_call(doit),
                                               choices=['1/11', '1/2', '1/1'],
                                               weight=1, value='1/1')
        myform['dac_gain'] = form.slider_field(parent=self.panel, sizer=hbox,
                                               callback=myform.check_input_and_call(doit),
                                               min=-32, max=31, value=0,
                                               weight=4)


    # ----------------------------------------------------------------

    def _create_pga(self):
        sbs = wx.StaticBoxSizer(wx.StaticBox(self.panel), wx.VERTICAL)

        form.quantized_slider_field(parent=self.panel, sizer=sbs, label="PGA",
                                    weight=3, range=self.subdev.gain_range(),
                                    callback=self.subdev.set_gain)

        self.vbox.Add(sbs, 0, wx.EXPAND)


    # ----------------------------------------------------------------

    
    def _set_status_msg(self, msg):
        self.GetStatusBar().SetStatusText(msg, 0)

        
if False and __name__ == '__main__':

    class demo_app (wx.App):
        def __init__ (self):
            wx.App.__init__(self)

        def OnInit (self):
            frame = tx_debug_gui(None, "Debug TX")
            frame.Show(True)
            self.SetTopWindow (frame)
            return True

    app = demo_app()
    app.MainLoop()
