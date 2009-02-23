#!/usr/bin/env python
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
from gnuradio.wxgui import form, slider, gui
import usrp2_siggen
import sys, math

class app_gui(object):
    def __init__(self, frame, panel, vbox, top_block, options, args):
        self.frame = frame      # Use for top-level application window frame
        self.panel = panel      # Use as parent class for created windows
        self.vbox = vbox        # Use as sizer for created windows
        self.tb = top_block     # GUI-unaware flowgraph class
        self.options = options  # Supplied command-line options
        self.args = args        # Supplied command-line arguments

        freq_range = self.tb.freq_range()
        self.min_freq = freq_range[0]
        self.max_freq = freq_range[1]
        self.freq_step = (self.max_freq-self.min_freq)/100.0
        self._types = dict([v, k] for k, v in usrp2_siggen.waveforms.items())

        self.build_gui()

        # TODO: turn these into listeners
        self.myform['ifc'].set_value(self.tb.interface_name())
        self.myform['mac'].set_value(self.tb.mac_addr())
        dbid = self.tb.daughterboard_id()
        self.myform['dbid'].set_value("%04x" % (dbid,))

        w = usrp2_siggen.waveforms[self.tb.waveform_type()]
        self.myform['type'].set_value(w)
        self.myform['w1freq'].set_value(self.tb.waveform_freq())
        self.myform['w2freq'].set_value(self.tb.waveform2_freq())

        freq = self.tb.freq()
        if freq is None:
            self.evt_set_status_msg("Failed to set initial frequency")
        else:
            self.myform['freq'].set_value(freq)
            self.myform['freq_slider'].set_value(self.tb.freq())

        amp = self.tb.amplitude()
        if (amp > 0.0):
            db = 20*math.log10(amp)
        else:
            db = -100.0
        self.myform['amp'].set_value(amp)
        self.myform['amp_slider'].set_value(db)
        self.myform['eth'].set_value(self.tb.eth_rate())
        self.myform['gbe'].set_value(self.tb.eth_rate()*32)
        self.myform['interp'].set_value(self.tb.interp_rate())
        self.myform['DDC'].set_value(self.tb.ddc_freq())
        self.myform['analog'].set_value(self.tb.baseband_freq())

    # Event response handlers
    def evt_set_status_msg(self, msg):
        self.frame.SetStatusText(msg, 0)

    def evt_set_freq1(self, kv):
        return self.tb.set_waveform_freq(kv['w1freq'])

    def evt_set_freq2(self, kv):
        return self.tb.set_waveform2_freq(kv['w2freq'])

    def evt_set_freq(self, kv):
        if type(kv) == type(0.0):              # Set from slider
            tr = self.tb.set_freq(kv)
            if tr is not None:
                self.myform['freq'].set_value(kv)
        else:                                  # Set from edit box
            f = kv['freq']
            tr = self.tb.set_freq(f)
            if tr is not None:
                self.myform['freq_slider'].set_value(f)

        if tr is not None:
            self.myform['DDC'].set_value(tr.dxc_freq)
            self.myform['analog'].set_value(tr.baseband_freq)

        return (tr is not None)

    def evt_set_amplitude(self, kv):
        if type(kv) == type(0.0):              # Set from slider
            amp = math.pow(10, kv/20.0)
            self.myform['amp'].set_value(amp)
            return self.tb.set_amplitude(amp)
        else:                                  # Set from edit box
            amp = kv['amp']
            if amp < 0.0 or amp > 1.0:
                return False
            if amp == 0.0:
                db = -100.0
            else:
                db = 20*math.log10(amp)
            self.myform['amp_slider'].set_value(db)
            return self.tb.set_amplitude(amp)

    def evt_set_interp(self):
        interp = self.myform['interp'].get_value()
        if self.tb.set_interp(interp):
            eth_rate = self.tb.eth_rate()
            self.myform['eth'].set_value(eth_rate)
            self.myform['gbe'].set_value(eth_rate*32)
            return True
        return False

    def evt_set_waveform_type(self, type):
        # TODO: update frequency labels
        return self.tb.set_waveform(self._types[type])

    # GUI construction
    def build_gui(self):
        self.myform = myform = form.form()

        # Baseband controls
        bb_sbox = wx.StaticBox(parent=self.panel, label="Baseband Modulation")
        bb_vbox = wx.StaticBoxSizer(bb_sbox, wx.VERTICAL) # Holds all baseband controls as unit

        # First row of baseband controls (modulation type)
        mod_hbox = wx.BoxSizer(wx.HORIZONTAL)
        mod_hbox.Add((10,0), 0, 0)
        myform['type'] = form.radiobox_field(
            parent=self.panel, label="Type", sizer=mod_hbox, value=None,
            callback=self.evt_set_waveform_type, weight=1, major_dimension=0,
            choices=usrp2_siggen.waveforms.values() )
        bb_vbox.Add((0,10), 0, 0)
        bb_vbox.Add(mod_hbox, 0, wx.EXPAND)

        # Second row of baseband controls (frequencies)
        bbf_hbox = wx.BoxSizer(wx.HORIZONTAL)
        bbf_hbox.Add((10,0), 0, 0)
        myform['w1freq'] = form.float_field(
            parent=self.panel, sizer=bbf_hbox, label="Frequency 1 (Hz)", weight=1,
            callback=myform.check_input_and_call(self.evt_set_freq1, self.evt_set_status_msg) )
        bbf_hbox.Add((10,0), 0, 0)
        myform['w2freq'] = form.float_field(
            parent=self.panel, sizer=bbf_hbox, label="Frequency 2 (Hz)", weight=1,
            callback=myform.check_input_and_call(self.evt_set_freq2, self.evt_set_status_msg) )
        bbf_hbox.Add((10,0), 0, 0)
        
        bb_vbox.Add((0,10), 0, 0)
        bb_vbox.Add(bbf_hbox, 0, wx.EXPAND)

        # Add baseband controls to top window sizer
        self.vbox.Add((0,10), 0, 0)
        self.vbox.Add(bb_vbox, 0, wx.EXPAND)

        # Frequency controls
        fc_sbox = wx.StaticBox(parent=self.panel, label="Center Frequency")
        fc_vbox = wx.StaticBoxSizer(fc_sbox, wx.VERTICAL) # Holds all frequency controls as unit

        # First row of frequency controls (center frequency)
        freq_hbox = wx.BoxSizer(wx.HORIZONTAL)
        freq_hbox.Add((10,0), 0, 0)
        myform['freq'] = form.float_field(
            parent=self.panel, sizer=freq_hbox, label=None, weight=1,
            callback=myform.check_input_and_call(self.evt_set_freq, self.evt_set_status_msg) )
        freq_hbox.Add((10,0), 0, 0)
        myform['freq_slider'] = form.quantized_slider_field(
            parent=self.panel, sizer=freq_hbox, label="Min-Max", weight=4,
            range = (self.min_freq, self.max_freq, self.freq_step),
            callback=self.evt_set_freq)
        freq_hbox.Add((10,0), 0, 0)

        fc_vbox.Add((10,0), 0, 0)
        fc_vbox.Add(freq_hbox, 0, wx.EXPAND)

        # Second row of frequency controls (results)
        tr_hbox = wx.BoxSizer(wx.HORIZONTAL)
        tr_hbox.Add((10,0), 0, 0)
        myform['analog'] = form.static_float_field(
            parent=self.panel, sizer=tr_hbox, label="Daughterboard: (Hz)", weight=1)
        tr_hbox.Add((10,0), 0, 0)
        myform['DDC'] = form.static_float_field(
            parent=self.panel, sizer=tr_hbox, label="USRP2 DDC (Hz)", weight=1)
        tr_hbox.Add((10,0), 0, 0)
        fc_vbox.Add(tr_hbox, 0, wx.EXPAND)

        # Add frequency controls to top window sizer
        self.vbox.Add((0,10), 0, 0)
        self.vbox.Add(fc_vbox, 0, wx.EXPAND)

        # Amplitude row
        amp_sbox = wx.StaticBox(parent=self.panel, label="Amplitude")
        amp_hbox = wx.StaticBoxSizer(amp_sbox, wx.HORIZONTAL)
        amp_hbox.Add((10,0), 0, 0)
        myform['amp'] = form.float_field(
            parent=self.panel, sizer=amp_hbox, label="Linear\n(0.0-1.0)", weight=1,
            callback=myform.check_input_and_call(self.evt_set_amplitude, self.evt_set_status_msg) )
        amp_hbox.Add((10,0), 0, 0)
        myform['amp_slider'] = form.quantized_slider_field(
            parent=self.panel, sizer=amp_hbox, label="dB Full Scale\n(-100-0)", weight=4,
            range=(-100.0, 0.0, 1), callback=self.evt_set_amplitude)
        amp_hbox.Add((10,0), 0, 0)
        self.vbox.Add((0,10), 0, 0)
        self.vbox.Add(amp_hbox, 0, wx.EXPAND)

        # Sample rate row
        sam_sbox = wx.StaticBox(parent=self.panel, label="Sample Rate")
        sam_hbox = wx.StaticBoxSizer(sam_sbox, wx.HORIZONTAL)
        sam_hbox.Add((10,0), 0, 0)
        myform['interp'] = form.int_field(
            parent=self.panel, sizer=sam_hbox, label="Interpolation", weight=1,
            callback=self.evt_set_interp)
        sam_hbox.Add((10,0), 0, 0)
        myform['eth'] = form.static_float_field(
            parent=self.panel, sizer=sam_hbox, label="Sample Rate (sps)", weight=1)
        sam_hbox.Add((10,0), 0, 0)
        myform['gbe'] = form.static_float_field(
            parent=self.panel, sizer=sam_hbox, label="GbE Rate (bits/sec)", weight=1)
        sam_hbox.Add((10,0), 0, 0)
        self.vbox.Add((0,10), 0, 0)
        self.vbox.Add(sam_hbox, 0, wx.EXPAND)

        # USRP2 row
        u2_sbox = wx.StaticBox(parent=self.panel, label="USRP2 Hardware")
        u2_hbox = wx.StaticBoxSizer(u2_sbox, wx.HORIZONTAL)
        u2_hbox.Add((10,0), 0, 0)
        myform['ifc'] = form.static_text_field(parent=self.panel, sizer=u2_hbox, 
                                               label="Interface", weight=2)
        u2_hbox.Add((10,0), 0, 0)
        myform['mac'] = form.static_text_field(parent=self.panel, sizer=u2_hbox, 
                                               label="MAC Address", weight=2)
        u2_hbox.Add((10,0), 0, 0)
        myform['dbid'] = form.static_text_field(parent=self.panel, sizer=u2_hbox, 
                                                label="Daughterboard ID", weight=1)
        self.vbox.Add((0,10), 0, 0)
        self.vbox.Add(u2_hbox, 0, wx.EXPAND)
        self.vbox.Add((0,20), 0, 0)

if __name__ == "__main__":
    try:
        # Get command line parameters
        (options, args) = usrp2_siggen.get_options()

        # Create the top block using these
        tb = usrp2_siggen.top_block(options, args)

        # Create the GUI application
        app = gui.app(top_block=tb,                    # Constructed top block
                      gui=app_gui,                     # User interface class
                      options=options,                 # Command line options
                      args=args,                       # Command line args
                      title="USRP2 Signal Generator",  # Top window title
                      nstatus=1,                       # Number of status lines
                      start=True,                      # Whether to start flowgraph
                      realtime=True)                   # Whether to set realtime priority

        # And run it
        app.MainLoop()

    except RuntimeError, e:
        print e
        sys.exit(1)
