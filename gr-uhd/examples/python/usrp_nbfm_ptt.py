#!/usr/bin/env python
#
# Copyright 2005,2007,2011 Free Software Foundation, Inc.
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

from gnuradio import gr, audio, uhd
from gnuradio import analog
from gnuradio import blocks
from gnuradio import filter
from gnuradio.eng_option import eng_option
from gnuradio.wxgui import stdgui2, fftsink2, scopesink2, slider, form

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
        parser.add_option("-a", "--args", type="string", default="",
                          help="UHD device address args [default=%default]")
        parser.add_option("", "--spec", type="string", default=None,
	                  help="Subdevice of UHD device where appropriate")
        parser.add_option("-A", "--antenna", type="string", default=None,
                          help="select Rx Antenna where appropriate")
        parser.add_option ("-f", "--freq", type="eng_float", default=442.1e6,
                           help="set Tx and Rx frequency to FREQ", metavar="FREQ")
        parser.add_option ("-g", "--rx-gain", type="eng_float", default=None,
                           help="set rx gain [default=midpoint in dB]")
        parser.add_option ("", "--tx-gain", type="eng_float", default=None,
                           help="set tx gain [default=midpoint in dB]")
        parser.add_option("-I", "--audio-input", type="string", default="default",
                          help="pcm input device name.  E.g., hw:0,0 or /dev/dsp")
        parser.add_option("-O", "--audio-output", type="string", default="default",
                          help="pcm output device name.  E.g., hw:0,0 or /dev/dsp")
        parser.add_option ("-N", "--no-gui", action="store_true", default=False)
        (options, args) = parser.parse_args ()

        if len(args) != 0:
            parser.print_help()
            sys.exit(1)

        if options.freq < 1e6:
            options.freq *= 1e6

        self.txpath = transmit_path(options.args, options.spec,
                                    options.antenna, options.tx_gain,
                                    options.audio_input)
        self.rxpath = receive_path(options.args, options.spec,
                                   options.antenna, options.rx_gain,
                                   options.audio_output)
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
            rx_fft = fftsink2.fft_sink_c(panel, title="Post s/w Resampler",
                                         fft_size=512, sample_rate=self.rxpath.quad_rate,
                                         ref_level=80, y_per_div=20)
            self.connect (self.rxpath.resamp, rx_fft)
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

        g = self.rxpath.u.get_gain_range()
        hbox.Add((5,0), 0)
        myform['rx_gain'] = \
            form.quantized_slider_field(parent=self.panel, sizer=hbox, label="Rx Gain",
                                        weight=3, range=(g.start(), g.stop(), g.step()),
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
    def __init__(self, args, spec, antenna, gain, audio_input):
	gr.hier_block2.__init__(self, "transmit_path",
				gr.io_signature(0, 0, 0), # Input signature
				gr.io_signature(0, 0, 0)) # Output signature

        self.u = uhd.usrp_sink(device_addr=args, stream_args=uhd.stream_args('fc32'))

        # Set the subdevice spec
        if(spec):
            self.u.set_subdev_spec(spec, 0)

        # Set the antenna
        if(antenna):
            self.u.set_antenna(antenna, 0)

        self.if_rate = 320e3
        self.audio_rate = 32e3

        self.u.set_samp_rate(self.if_rate)
        dev_rate = self.u.get_samp_rate()

        self.audio_gain = 10
        self.normal_gain = 32000

        self.audio = audio.source(int(self.audio_rate), audio_input)
        self.audio_amp = blocks.multiply_const_ff(self.audio_gain)

        lpf = filter.firdes.low_pass(1,                  # gain
                                     self.audio_rate,    # sampling rate
                                     3800,               # low pass cutoff freq
                                     300,                # width of trans. band
                                     filter.firdes.WIN_HANN) # filter type

        hpf = filter.firdes.high_pass(1,                  # gain
                                      self.audio_rate,    # sampling rate
                                      325,                # low pass cutoff freq
                                      50,                 # width of trans. band
                                      filter.firdes.WIN_HANN) # filter type

        audio_taps = convolve(array(lpf),array(hpf))
        self.audio_filt = filter.fir_filter_fff(1,audio_taps)

        self.pl = analog.ctcss_gen_f(self.audio_rate,123.0)
        self.add_pl = blocks.add_ff()
        self.connect(self.pl,(self.add_pl,1))

        self.fmtx = analog.nbfm_tx(self.audio_rate, self.if_rate)
        self.amp = blocks.multiply_const_cc (self.normal_gain)

        rrate = dev_rate / self.if_rate
        self.resamp = filter.pfb.arb_resampler_ccf(rrate)

        self.connect(self.audio, self.audio_amp, self.audio_filt,
                     (self.add_pl,0), self.fmtx, self.amp,
                     self.resamp, self.u)

        if gain is None:
            # if no gain was specified, use the mid-point in dB
            g = self.u.get_gain_range()
            gain = float(g.start() + g.stop())/2.0

        self.set_gain(gain)

        self.set_enable(False)

    def set_freq(self, target_freq):
        """
        Set the center frequency we're interested in.

        Args:
            target_freq: frequency in Hz
        @rypte: bool
        """
        r = self.u.set_center_freq(target_freq)
        if r:
            return True
        return False

    def set_gain(self, gain):
        self.gain = gain
        self.u.set_gain(gain)

    def set_enable(self, enable):
        if enable:
            self.amp.set_k (self.normal_gain)
        else:
            self.amp.set_k (0)



# ////////////////////////////////////////////////////////////////////////
#                           Receive Path
# ////////////////////////////////////////////////////////////////////////

class receive_path(gr.hier_block2):
    def __init__(self, args, spec, antenna, gain, audio_output):
	gr.hier_block2.__init__(self, "receive_path",
				gr.io_signature(0, 0, 0), # Input signature
				gr.io_signature(0, 0, 0)) # Output signature

        self.u = uhd.usrp_source(device_addr=args,
                                 io_type=uhd.io_type.COMPLEX_FLOAT32,
                                 num_channels=1)

        self.if_rate    = 256e3
        self.quad_rate  = 64e3
        self.audio_rate = 32e3

        self.u.set_samp_rate(self.if_rate)
        dev_rate = self.u.get_samp_rate()

        # Create filter to get actual channel we want
        nfilts = 32
        chan_coeffs = filter.firdes.low_pass(nfilts,             # gain
                                             nfilts*dev_rate,    # sampling rate
                                             13e3,               # low pass cutoff freq
                                             4e3,                # width of trans. band
                                             filter.firdes.WIN_HANN) # filter type

        rrate = self.quad_rate / dev_rate
        self.resamp = filter.pfb.arb_resampler_ccf(rrate, chan_coeffs, nfilts)

        # instantiate the guts of the single channel receiver
        self.fmrx = analog.nbfm_rx(self.audio_rate, self.quad_rate)

        # standard squelch block
        self.squelch = analog.standard_squelch(self.audio_rate)

        # audio gain / mute block
        self._audio_gain = blocks.multiply_const_ff(1.0)

        # sound card as final sink
        audio_sink = audio.sink(int(self.audio_rate), audio_output)

        # now wire it all together
        self.connect(self.u, self.resamp, self.fmrx, self.squelch,
                     self._audio_gain, audio_sink)

        if gain is None:
            # if no gain was specified, use the mid-point in dB
            g = self.u.get_gain_range()
            gain = float(g.start() + g.stop())/2.0

        self.enabled = True
        self.set_gain(gain)
        v = self.volume_range()
        self.set_volume((v[0]+v[1])/2)
        s = self.squelch_range()
        self.set_squelch((s[0]+s[1])/2)

        # Set the subdevice spec
        if(spec):
            self.u.set_subdev_spec(spec, 0)

        # Set the antenna
        if(antenna):
            self.u.set_antenna(antenna, 0)

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

        Args:
            target_freq: frequency in Hz
        @rypte: bool
        """
        r = self.u.set_center_freq(target_freq)
        if r:
            return True
        return False

    def set_gain(self, gain):
        self.gain = gain
        self.u.set_gain(gain)


# ////////////////////////////////////////////////////////////////////////
#                                Main
# ////////////////////////////////////////////////////////////////////////

def main():
    app = stdgui2.stdapp(ptt_block, "NBFM Push to Talk")
    app.MainLoop()

if __name__ == '__main__':
    main()
