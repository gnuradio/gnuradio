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
from gnuradio import gr
from gnuradio.gr.pubsub import pubsub
from gnuradio.wxgui import gui, forms
import usrp_siggen
import sys, math

class app_gui(pubsub):
    def __init__(self, frame, panel, vbox, top_block, options, args):
        pubsub.__init__(self)
        self.frame = frame      # Use for top-level application window frame
        self.panel = panel      # Use as parent class for created windows
        self.vbox = vbox        # Use as sizer for created windows
        self.tb = top_block     # GUI-unaware flowgraph class
        self.options = options  # Supplied command-line options
        self.args = args        # Supplied command-line arguments
        self.build_gui()

    # Event response handlers
    def evt_set_status_msg(self, msg):
        self.frame.SetStatusText(msg, 0)

    # GUI construction
    def build_gui(self):
        self.vbox.AddSpacer(5)
        self.vbox.AddStretchSpacer()
        ##################################################
        # Baseband controls
        ##################################################
        bb_vbox = forms.static_box_sizer(parent=self.panel, label="Baseband Modulation", orient=wx.VERTICAL, bold=True)
        self.vbox.Add(bb_vbox, 0, wx.EXPAND)
        sine_bb_hbox = wx.BoxSizer(wx.HORIZONTAL)
        sweep_bb_hbox = wx.BoxSizer(wx.HORIZONTAL)
        tone_bb_hbox = wx.BoxSizer(wx.HORIZONTAL)
        self.vbox.AddSpacer(10)
        self.vbox.AddStretchSpacer()
        #callback to show/hide forms
        def set_type(type):
            sine_bb_hbox.ShowItems(type == gr.GR_SIN_WAVE)
            sweep_bb_hbox.ShowItems(type == 'sweep')
            tone_bb_hbox.ShowItems(type == '2tone')
            self.vbox.Layout()
        self.tb.subscribe(usrp_siggen.TYPE_KEY, set_type)
        #create sine forms
        sine_bb_hbox.AddSpacer(10)
        forms.text_box(
            parent=self.panel, sizer=sine_bb_hbox,
            label='Frequency (Hz)',
            ps=self.tb,
            key=usrp_siggen.WAVEFORM_FREQ_KEY,
            converter=forms.float_converter(),
        )
        sine_bb_hbox.AddStretchSpacer()
        #create sweep forms
        sweep_bb_hbox.AddSpacer(10)
        forms.text_box(
            parent=self.panel, sizer=sweep_bb_hbox,
            label='Sweep Width (Hz)',
            ps=self.tb,
            key=usrp_siggen.WAVEFORM_FREQ_KEY,
            converter=forms.float_converter(),
        )
        sweep_bb_hbox.AddStretchSpacer()
        forms.text_box(
            parent=self.panel, sizer=sweep_bb_hbox,
            label='Sweep Rate (Hz)',
            ps=self.tb,
            key=usrp_siggen.WAVEFORM2_FREQ_KEY,
            converter=forms.float_converter(),
        )
        sweep_bb_hbox.AddStretchSpacer()
        #create 2tone forms
        tone_bb_hbox.AddSpacer(10)
        forms.text_box(
            parent=self.panel, sizer=tone_bb_hbox,
            label='Tone 1 (Hz)',
            ps=self.tb,
            key=usrp_siggen.WAVEFORM_FREQ_KEY,
            converter=forms.float_converter(),
        )
        tone_bb_hbox.AddStretchSpacer()
        forms.text_box(
            parent=self.panel, sizer=tone_bb_hbox,
            label='Tone 2 (Hz)',
            ps=self.tb,
            key=usrp_siggen.WAVEFORM2_FREQ_KEY,
            converter=forms.float_converter(),
        )
        tone_bb_hbox.AddStretchSpacer()
        forms.radio_buttons(
            parent=self.panel, sizer=bb_vbox,
            choices=usrp_siggen.waveforms.keys(),
            labels=usrp_siggen.waveforms.values(),
            ps=self.tb,
            key=usrp_siggen.TYPE_KEY,
            style=wx.NO_BORDER | wx.RA_HORIZONTAL,
        )
        bb_vbox.AddSpacer(10)
        bb_vbox.Add(sine_bb_hbox, 0, wx.EXPAND)
        bb_vbox.Add(sweep_bb_hbox, 0, wx.EXPAND)
        bb_vbox.Add(tone_bb_hbox, 0, wx.EXPAND)
        set_type(self.tb[usrp_siggen.TYPE_KEY])
        ##################################################
        # Frequency controls
        ##################################################
        fc_vbox = forms.static_box_sizer(parent=self.panel, label="Center Frequency", orient=wx.VERTICAL, bold=True)
        fc_vbox.AddSpacer(5)
        # First row of frequency controls (center frequency)
        freq_hbox = wx.BoxSizer(wx.HORIZONTAL)
        fc_vbox.Add(freq_hbox, 0, wx.EXPAND)
        fc_vbox.AddSpacer(10)
        # Second row of frequency controls (results)
        tr_hbox = wx.BoxSizer(wx.HORIZONTAL)
        fc_vbox.Add(tr_hbox, 0, wx.EXPAND)
        fc_vbox.AddSpacer(5)
        # Add frequency controls to top window sizer
        self.vbox.Add(fc_vbox, 0, wx.EXPAND)
        self.vbox.AddSpacer(10)
        self.vbox.AddStretchSpacer()
        freq_hbox.AddSpacer(5)
        forms.text_box(
            parent=self.panel, sizer=freq_hbox,
            proportion=1,
            converter=forms.float_converter(),
            ps=self.tb,
            key=usrp_siggen.TX_FREQ_KEY,
        )
        freq_hbox.AddSpacer(10)
        forms.slider(
            parent=self.panel, sizer=freq_hbox,
            proportion=2,
            ps=self.tb,
            key=usrp_siggen.TX_FREQ_KEY,
            minimum=self.tb[usrp_siggen.FREQ_RANGE_KEY][0],
            maximum=self.tb[usrp_siggen.FREQ_RANGE_KEY][1],
            num_steps=100,
        )
        freq_hbox.AddSpacer(5)
        tr_hbox.AddSpacer(5)
        forms.static_text(
            parent=self.panel, sizer=tr_hbox,
            label='Daughterboard (Hz)',
            ps=self.tb,
            key=usrp_siggen.BB_FREQ_KEY,
            converter=forms.float_converter(),
            proportion=1,
        )
        tr_hbox.AddSpacer(10)
        forms.static_text(
            parent=self.panel, sizer=tr_hbox,
            label='USRP DDC (Hz)',
            ps=self.tb,
            key=usrp_siggen.DDC_FREQ_KEY,
            converter=forms.float_converter(),
            proportion=1,
        )
        tr_hbox.AddSpacer(5)
        ##################################################
        # Amplitude controls
        ##################################################
        amp_hbox = forms.static_box_sizer(parent=self.panel, label="Amplitude", orient=wx.VERTICAL, bold=True)
        amp_hbox.AddSpacer(5)
        # First row of amp controls (ampl)
        lvl_hbox = wx.BoxSizer(wx.HORIZONTAL)
        amp_hbox.Add(lvl_hbox, 0, wx.EXPAND)
        amp_hbox.AddSpacer(10)
        # Second row of amp controls (tx gain)
        gain_hbox = wx.BoxSizer(wx.HORIZONTAL)
        amp_hbox.Add(gain_hbox, 0, wx.EXPAND)
        amp_hbox.AddSpacer(5)
        self.vbox.Add(amp_hbox, 0, wx.EXPAND)
        self.vbox.AddSpacer(10)
        self.vbox.AddStretchSpacer()
        lvl_hbox.AddSpacer(5)
        forms.text_box(
            parent=self.panel, sizer=lvl_hbox,
            proportion=1,
            converter=forms.float_converter(),
            ps=self.tb,
            key=usrp_siggen.AMPLITUDE_KEY,
            label="Level (0.0-1.0)",
        )
        lvl_hbox.AddSpacer(10)
        forms.log_slider(
            parent=self.panel, sizer=lvl_hbox,
            proportion=2,
            ps=self.tb,
            key=usrp_siggen.AMPLITUDE_KEY,
            min_exp=-5,
            max_exp=0,
            base=10,
            step_size=1,
        )
        lvl_hbox.AddSpacer(5)
        if self.tb[usrp_siggen.GAIN_RANGE_KEY][0] < self.tb[usrp_siggen.GAIN_RANGE_KEY][1]:
            gain_hbox.AddSpacer(5)
            forms.text_box(
                parent=self.panel, sizer=gain_hbox,
                proportion=1,
                converter=forms.float_converter(),
                ps=self.tb,
                key=usrp_siggen.GAIN_KEY,
                label="TX Gain (dB)",
            )
            gain_hbox.AddSpacer(10)
            forms.slider(
                parent=self.panel, sizer=gain_hbox,
                proportion=2,
                ps=self.tb,
                key=usrp_siggen.GAIN_KEY,
                minimum=self.tb[usrp_siggen.GAIN_RANGE_KEY][0],
                maximum=self.tb[usrp_siggen.GAIN_RANGE_KEY][1],
                step_size=self.tb[usrp_siggen.GAIN_RANGE_KEY][2],
            )
            gain_hbox.AddSpacer(5)
        ##################################################
        # Sample Rate controls
        ##################################################
        sam_hbox = forms.static_box_sizer(parent=self.panel, label="Sample Rate", orient=wx.HORIZONTAL, bold=True)
        self.vbox.Add(sam_hbox, 0, wx.EXPAND)
        self.vbox.AddSpacer(10)
        self.vbox.AddStretchSpacer()
        sam_hbox.AddSpacer(5)
        forms.text_box(
            parent=self.panel, sizer=sam_hbox,
            converter=forms.int_converter(),
            ps=self.tb,
            key=usrp_siggen.INTERP_KEY,
            label="Interpolation",
        )
        sam_hbox.AddStretchSpacer(20)
        forms.static_text(
            parent=self.panel, sizer=sam_hbox,
            label='Sample Rate (sps)',
            ps=self.tb,
            key=usrp_siggen.SAMP_RATE_KEY,
            converter=forms.float_converter(),
        )
        sam_hbox.AddStretchSpacer(20)
        forms.static_text(
            parent=self.panel, sizer=sam_hbox,
            label='Link Rate (bits/sec)',
            ps=self.tb,
            key=usrp_siggen.LINK_RATE_KEY,
            converter=forms.float_converter(),
        )
        sam_hbox.AddSpacer(5)
        ##################################################
        # USRP status
        ##################################################
        u2_hbox = forms.static_box_sizer(parent=self.panel, label="USRP Status", orient=wx.HORIZONTAL, bold=True)
        self.vbox.Add(u2_hbox, 0, wx.EXPAND)
        self.vbox.AddSpacer(10)
        self.vbox.AddStretchSpacer()
        u2_hbox.AddSpacer(10)
        forms.static_text(
            parent=self.panel, sizer=u2_hbox,
            ps=self.tb,
            key=usrp_siggen.DESC_KEY,
            converter=forms.str_converter(),
        )
        self.vbox.AddSpacer(5)
        self.vbox.AddStretchSpacer()

if __name__ == "__main__":
    try:
        # Get command line parameters
        (options, args) = usrp_siggen.get_options()

        # Create the top block using these
        tb = usrp_siggen.top_block(options, args)

        # Create the GUI application
        app = gui.app(top_block=tb,                    # Constructed top block
                      gui=app_gui,                     # User interface class
                      options=options,                 # Command line options
                      args=args,                       # Command line args
                      title="USRP Signal Generator",  # Top window title
                      nstatus=1,                       # Number of status lines
                      start=True,                      # Whether to start flowgraph
                      realtime=True)                   # Whether to set realtime priority

        # And run it
        app.MainLoop()

    except RuntimeError, e:
        print e
        sys.exit(1)
