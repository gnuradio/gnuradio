#!/usr/bin/env python
#
# Copyright 2005,2007 Free Software Foundation, Inc.
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

import math
import sys
import wx
from optparse import OptionParser

from gnuradio import gr, gru, eng_notation
from gnuradio import usrp
from gnuradio import audio
from gnuradio import blks2
from gnuradio.eng_option import eng_option
from gnuradio.wxgui import stdgui2, fftsink2, scopesink2, slider, form
from usrpm import usrp_dbid

from numpy import convolve, array

#import os
#print "pid =", os.getpid()
#raw_input('Press Enter to continue: ')

# ////////////////////////////////////////////////////////////////////////
#                           Control Stuff
# ////////////////////////////////////////////////////////////////////////

class ptt_block(stdgui2.std_top_block):
    def __init__(self, frame, panel, vbox, argv):
        stdgui2.std_top_block.__init__ (self, frame, panel, vbox, argv)

        self.frame = frame
        self.space_bar_pressed = False
        
        parser = OptionParser (option_class=eng_option)
        parser.add_option("-R", "--rx-subdev-spec", type="subdev", default=None,
                          help="select USRP Rx side A or B")
        parser.add_option("-T", "--tx-subdev-spec", type="subdev", default=None,
                          help="select USRP Tx side A or B")
        parser.add_option ("-f", "--freq", type="eng_float", default=442.1e6,
                           help="set Tx and Rx frequency to FREQ", metavar="FREQ")
        parser.add_option ("-g", "--rx-gain", type="eng_float", default=None,
                           help="set rx gain [default=midpoint in dB]")
        parser.add_option("-I", "--audio-input", type="string", default="",
                          help="pcm input device name.  E.g., hw:0,0 or /dev/dsp")
        parser.add_option("-O", "--audio-output", type="string", default="",
                          help="pcm output device name.  E.g., hw:0,0 or /dev/dsp")
        parser.add_option ("-N", "--no-gui", action="store_true", default=False)
        (options, args) = parser.parse_args ()

        if len(args) != 0:
            parser.print_help()
            sys.exit(1)

        if options.freq < 1e6:
            options.freq *= 1e6
            
        self.txpath = transmit_path(options.tx_subdev_spec, options.audio_input)
        self.rxpath = receive_path(options.rx_subdev_spec, options.rx_gain, options.audio_output)
	self.connect(self.txpath)
	self.connect(self.rxpath)

        self._build_gui(frame, panel, vbox, argv, options.no_gui)

        self.set_transmit(False)
        self.set_freq(options.freq)
        self.set_rx_gain(self.rxpath.gain)               # update gui
        self.set_volume(self.rxpath.volume)              # update gui
        self.set_squelch(self.rxpath.threshold())        # update gui


    def set_transmit(self, enabled):
        self.txpath.set_enable(enabled)
        self.rxpath.set_enable(not(enabled))
        if enabled:
            self.frame.SetStatusText ("Transmitter ON", 1)
        else:
            self.frame.SetStatusText ("Receiver ON", 1)


    def set_rx_gain(self, gain):
        self.myform['rx_gain'].set_value(gain)            # update displayed value
        self.rxpath.set_gain(gain)
        
    def set_tx_gain(self, gain):
        self.txpath.set_gain(gain)

    def set_squelch(self, threshold):
        self.rxpath.set_squelch(threshold)
        self.myform['squelch'].set_value(self.rxpath.threshold())

    def set_volume (self, vol):
        self.rxpath.set_volume(vol)
        self.myform['volume'].set_value(self.rxpath.volume)
        #self.update_status_bar ()

    def set_freq(self, freq):
        r1 = self.txpath.set_freq(freq)
        r2 = self.rxpath.set_freq(freq)
        #print "txpath.set_freq =", r1
        #print "rxpath.set_freq =", r2
        if r1 and r2:
            self.myform['freq'].set_value(freq)     # update displayed value
        return r1 and r2

    def _build_gui(self, frame, panel, vbox, argv, no_gui):

        def _form_set_freq(kv):
            return self.set_freq(kv['freq'])
            
        self.panel = panel
        
        # FIXME This REALLY needs to be replaced with a hand-crafted button
        # that sends both button down and button up events
        hbox = wx.BoxSizer(wx.HORIZONTAL)
        hbox.Add((10,0), 1)
        self.status_msg = wx.StaticText(panel, -1, "Press Space Bar to Transmit")
        of = self.status_msg.GetFont()
        self.status_msg.SetFont(wx.Font(15, of.GetFamily(), of.GetStyle(), of.GetWeight()))
        hbox.Add(self.status_msg, 0, wx.ALIGN_CENTER)
        hbox.Add((10,0), 1)
        vbox.Add(hbox, 0, wx.EXPAND | wx.ALIGN_CENTER)

        panel.Bind(wx.EVT_KEY_DOWN, self._on_key_down)
        panel.Bind(wx.EVT_KEY_UP, self._on_key_up)
        panel.Bind(wx.EVT_KILL_FOCUS, self._on_kill_focus)
        panel.SetFocus()

        if 1 and not(no_gui):
            rx_fft = fftsink2.fft_sink_c(panel, title="Rx Input", fft_size=512,
                                         sample_rate=self.rxpath.if_rate,
                                         ref_level=80, y_per_div=20)
            self.connect (self.rxpath.u, rx_fft)
            vbox.Add (rx_fft.win, 1, wx.EXPAND)

        if 1 and not(no_gui):
            rx_fft = fftsink2.fft_sink_c(panel, title="Post s/w DDC",
                                         fft_size=512, sample_rate=self.rxpath.quad_rate,
                                         ref_level=80, y_per_div=20)
            self.connect (self.rxpath.ddc, rx_fft)
            vbox.Add (rx_fft.win, 1, wx.EXPAND)

        if 0 and not(no_gui):
            foo = scopesink2.scope_sink_f(panel, title="Squelch",
                                    	  sample_rate=32000)
            self.connect (self.rxpath.fmrx.div, (foo,0))
            self.connect (self.rxpath.fmrx.gate, (foo,1))
            self.connect (self.rxpath.fmrx.squelch_lpf, (foo,2))
            vbox.Add (foo.win, 1, wx.EXPAND)

        if 0 and not(no_gui):
            tx_fft = fftsink2.fft_sink_c(panel, title="Tx Output",
                                         fft_size=512, sample_rate=self.txpath.usrp_rate)
            self.connect (self.txpath.amp, tx_fft)
            vbox.Add (tx_fft.win, 1, wx.EXPAND)


        # add control area at the bottom

        self.myform = myform = form.form()

        # first row
        hbox = wx.BoxSizer(wx.HORIZONTAL)
        hbox.Add((5,0), 0, 0)
        myform['freq'] = form.float_field(
            parent=panel, sizer=hbox, label="Freq", weight=1,
            callback=myform.check_input_and_call(_form_set_freq, self._set_status_msg))

        hbox.Add((5,0), 0, 0)
        vbox.Add(hbox, 0, wx.EXPAND)


        # second row
        hbox = wx.BoxSizer(wx.HORIZONTAL)
        myform['volume'] = \
            form.quantized_slider_field(parent=self.panel, sizer=hbox, label="Volume",
                                        weight=3, range=self.rxpath.volume_range(),
                                        callback=self.set_volume)
        hbox.Add((5,0), 0)
        myform['squelch'] = \
            form.quantized_slider_field(parent=self.panel, sizer=hbox, label="Squelch",
                                        weight=3, range=self.rxpath.squelch_range(),
                                        callback=self.set_squelch)
        hbox.Add((5,0), 0)
        myform['rx_gain'] = \
            form.quantized_slider_field(parent=self.panel, sizer=hbox, label="Rx Gain",
                                        weight=3, range=self.rxpath.subdev.gain_range(),
                                        callback=self.set_rx_gain)
        hbox.Add((5,0), 0)
        vbox.Add(hbox, 0, wx.EXPAND)


        self._build_subpanel(vbox)

    def _build_subpanel(self, vbox_arg):
        # build a secondary information panel (sometimes hidden)

        # FIXME figure out how to have this be a subpanel that is always
        # created, but has its visibility controlled by foo.Show(True/False)
        
        #if not(self.show_debug_info):
        #    return

        panel = self.panel
        vbox = vbox_arg
        myform = self.myform

        #panel = wx.Panel(self.panel, -1)
        #vbox = wx.BoxSizer(wx.VERTICAL)

        hbox = wx.BoxSizer(wx.HORIZONTAL)
        hbox.Add((5,0), 0)
        #myform['decim'] = form.static_float_field(
        #    parent=panel, sizer=hbox, label="Decim")

        #hbox.Add((5,0), 1)
        #myform['fs@usb'] = form.static_float_field(
        #    parent=panel, sizer=hbox, label="Fs@USB")

        #hbox.Add((5,0), 1)
        #myform['dbname'] = form.static_text_field(
        #    parent=panel, sizer=hbox)

        hbox.Add((5,0), 0)
        vbox.Add(hbox, 0, wx.EXPAND)


    def _set_status_msg(self, msg, which=0):
        self.frame.GetStatusBar().SetStatusText(msg, which)

    def _on_key_down(self, evt):
        # print "key_down:", evt.m_keyCode
        if evt.m_keyCode == wx.WXK_SPACE and not(self.space_bar_pressed):
            self.space_bar_pressed = True
            self.set_transmit(True)

    def _on_key_up(self, evt):
        # print "key_up", evt.m_keyCode
        if evt.m_keyCode == wx.WXK_SPACE:
            self.space_bar_pressed = False
            self.set_transmit(False)

    def _on_kill_focus(self, evt):
        # if we lose the keyboard focus, turn off the transmitter
        self.space_bar_pressed = False
        self.set_transmit(False)
        

# ////////////////////////////////////////////////////////////////////////
#                           Transmit Path
# ////////////////////////////////////////////////////////////////////////

class transmit_path(gr.hier_block2):
    def __init__(self, subdev_spec, audio_input):
	gr.hier_block2.__init__(self, "transmit_path",
				gr.io_signature(0, 0, 0), # Input signature
				gr.io_signature(0, 0, 0)) # Output signature
				
        self.u = usrp.sink_c ()

        dac_rate = self.u.dac_rate();
        self.if_rate = 320e3                               # 320 kS/s
        self.usrp_interp = int(dac_rate // self.if_rate)
        self.u.set_interp_rate(self.usrp_interp)
        self.sw_interp = 10
        self.audio_rate = self.if_rate // self.sw_interp   #  32 kS/s

        self.audio_gain = 10
        self.normal_gain = 32000

        self.audio = audio.source(int(self.audio_rate), audio_input)
        self.audio_amp = gr.multiply_const_ff(self.audio_gain)

        lpf = gr.firdes.low_pass (1,                # gain
                                  self.audio_rate,            # sampling rate
                                  3800,               # low pass cutoff freq
                                  300,                # width of trans. band
                                  gr.firdes.WIN_HANN) # filter type 

        hpf = gr.firdes.high_pass (1,                # gain
                                  self.audio_rate,            # sampling rate
                                  325,               # low pass cutoff freq
                                  50,                # width of trans. band
                                  gr.firdes.WIN_HANN) # filter type 

        audio_taps = convolve(array(lpf),array(hpf))
        self.audio_filt = gr.fir_filter_fff(1,audio_taps)

        self.pl = blks2.ctcss_gen_f(self.audio_rate,123.0)
        self.add_pl = gr.add_ff()
        self.connect(self.pl,(self.add_pl,1))

        self.fmtx = blks2.nbfm_tx(self.audio_rate, self.if_rate)
        self.amp = gr.multiply_const_cc (self.normal_gain)

        # determine the daughterboard subdevice we're using
        if subdev_spec is None:
            subdev_spec = usrp.pick_tx_subdevice(self.u)
        self.u.set_mux(usrp.determine_tx_mux_value(self.u, subdev_spec))
        self.subdev = usrp.selected_subdev(self.u, subdev_spec)
        print "TX using", self.subdev.name()

        self.connect(self.audio, self.audio_amp, self.audio_filt,
                     (self.add_pl,0), self.fmtx, self.amp, self.u)

        self.set_gain(self.subdev.gain_range()[1])  # set max Tx gain


    def set_freq(self, target_freq):
        """
        Set the center frequency we're interested in.

        @param target_freq: frequency in Hz
        @rypte: bool

        Tuning is a two step process.  First we ask the front-end to
        tune as close to the desired frequency as it can.  Then we use
        the result of that operation and our target_frequency to
        determine the value for the digital up converter.  Finally, we feed
        any residual_freq to the s/w freq translater.
        """
        r = self.u.tune(self.subdev._which, self.subdev, target_freq)
        if r:
            # Use residual_freq in s/w freq translator
            return True

        return False

    def set_gain(self, gain):
        self.gain = gain
        self.subdev.set_gain(gain)

    def set_enable(self, enable):
        self.subdev.set_enable(enable)            # set H/W Tx enable
        if enable:
            self.amp.set_k (self.normal_gain)
        else:
            self.amp.set_k (0)



# ////////////////////////////////////////////////////////////////////////
#                           Receive Path
# ////////////////////////////////////////////////////////////////////////

class receive_path(gr.hier_block2):
    def __init__(self, subdev_spec, gain, audio_output):
	gr.hier_block2.__init__(self, "receive_path",
				gr.io_signature(0, 0, 0), # Input signature
				gr.io_signature(0, 0, 0)) # Output signature

        self.u = usrp.source_c ()
        adc_rate = self.u.adc_rate()

        self.if_rate = 256e3                         # 256 kS/s
        usrp_decim = int(adc_rate // self.if_rate)
        if_decim = 4
        self.u.set_decim_rate(usrp_decim)
        self.quad_rate = self.if_rate // if_decim    #  64 kS/s
        audio_decim = 2
        audio_rate = self.quad_rate // audio_decim   #  32 kS/s

        if subdev_spec is None:
            subdev_spec = usrp.pick_rx_subdevice(self.u)
        self.subdev = usrp.selected_subdev(self.u, subdev_spec)
        print "RX using", self.subdev.name()

        self.u.set_mux(usrp.determine_rx_mux_value(self.u, subdev_spec))

        # Create filter to get actual channel we want
        chan_coeffs = gr.firdes.low_pass (1.0,                # gain
                                          self.if_rate,       # sampling rate
                                          13e3,               # low pass cutoff freq
                                          4e3,                # width of trans. band
                                          gr.firdes.WIN_HANN) # filter type 

        print "len(rx_chan_coeffs) =", len(chan_coeffs)

        # Decimating Channel filter with frequency translation
        # complex in and out, float taps
        self.ddc = gr.freq_xlating_fir_filter_ccf(if_decim,       # decimation rate
                                                  chan_coeffs,    # taps
                                                  0,              # frequency translation amount
                                                  self.if_rate)   # input sample rate

        # instantiate the guts of the single channel receiver
        self.fmrx = blks2.nbfm_rx(audio_rate, self.quad_rate)

        # standard squelch block
        self.squelch = blks2.standard_squelch(audio_rate)

        # audio gain / mute block
        self._audio_gain = gr.multiply_const_ff(1.0)

        # sound card as final sink
        audio_sink = audio.sink (int(audio_rate), audio_output)
        
        # now wire it all together
        self.connect (self.u, self.ddc, self.fmrx, self.squelch, self._audio_gain, audio_sink)

        if gain is None:
            # if no gain was specified, use the mid-point in dB
            g = self.subdev.gain_range()
            gain = float(g[0]+g[1])/2

        self.enabled = True
        self.set_gain(gain)
        v = self.volume_range()
        self.set_volume((v[0]+v[1])/2)
        s = self.squelch_range()
        self.set_squelch((s[0]+s[1])/2)
        
        
    def volume_range(self):
        return (-20.0, 0.0, 0.5)

    def set_volume (self, vol):
        g = self.volume_range()
        self.volume = max(g[0], min(g[1], vol))
        self._update_audio_gain()

    def set_enable(self, enable):
        self.enabled = enable
        self._update_audio_gain()

    def _update_audio_gain(self):
        if self.enabled:
            self._audio_gain.set_k(10**(self.volume/10))
        else:
            self._audio_gain.set_k(0)

    def squelch_range(self):
        return self.squelch.squelch_range()
    
    def set_squelch(self, threshold):
        print "SQL =", threshold
        self.squelch.set_threshold(threshold)

    def threshold(self):
        return self.squelch.threshold()
    
    def set_freq(self, target_freq):
        """
        Set the center frequency we're interested in.

        @param target_freq: frequency in Hz
        @rypte: bool

        Tuning is a two step process.  First we ask the front-end to
        tune as close to the desired frequency as it can.  Then we use
        the result of that operation and our target_frequency to
        determine the value for the digital down converter in the
        FPGA.  Finally, we feed any residual_freq to the s/w freq
        translator.
        """
        r = self.u.tune(0, self.subdev, target_freq)
        if r:
            # Use residual_freq in s/w freq translater
            # print "residual_freq =", r.residual_freq
            self.ddc.set_center_freq(-r.residual_freq)
            return True

        return False

    def set_gain(self, gain):
        self.gain = gain
        self.subdev.set_gain(gain)


# ////////////////////////////////////////////////////////////////////////
#                                Main
# ////////////////////////////////////////////////////////////////////////

def main():
    app = stdgui2.stdapp(ptt_block, "NBFM Push to Talk")
    app.MainLoop()

if __name__ == '__main__':
    main()
