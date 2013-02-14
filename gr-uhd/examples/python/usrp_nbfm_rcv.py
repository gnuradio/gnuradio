#!/usr/bin/env python
#
# Copyright 2005,2007,2011,2012 Free Software Foundation, Inc.
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

from gnuradio import gr, audio, uhd
from gnuradio import blocks
from gnuradio import filter
from gnuradio import analog
from gnuradio.eng_option import eng_option
from gnuradio.wxgui import slider, powermate
from gnuradio.wxgui import stdgui2, fftsink2, form
from optparse import OptionParser
import sys
import math
import wx

#////////////////////////////////////////////////////////////////////////
#                           Control Stuff
#////////////////////////////////////////////////////////////////////////

class my_top_block (stdgui2.std_top_block):
    def __init__(self,frame,panel,vbox,argv):
        stdgui2.std_top_block.__init__ (self,frame,panel,vbox,argv)

        parser=OptionParser(option_class=eng_option)
        parser.add_option("-a", "--args", type="string", default="",
                          help="UHD device address args [default=%default]")
        parser.add_option("", "--spec", type="string", default=None,
	                  help="Subdevice of UHD device where appropriate")
        parser.add_option("-A", "--antenna", type="string", default=None,
                          help="select Rx Antenna where appropriate")
        parser.add_option("-f", "--freq", type="eng_float", default=146.585e6,
                          help="set frequency to FREQ", metavar="FREQ")
        parser.add_option("-g", "--gain", type="eng_float", default=None,
                          help="set gain in dB (default is midpoint)")
        parser.add_option("-V", "--volume", type="eng_float", default=None,
                          help="set volume (default is midpoint)")
        parser.add_option("-O", "--audio-output", type="string", default="default",
                          help="pcm device name.  E.g., hw:0,0 or surround51 or /dev/dsp")
        parser.add_option("-N", "--no-gui", action="store_true", default=False)

        (options, args) = parser.parse_args()
        if len(args) != 0:
            parser.print_help()
            sys.exit(1)

        if options.freq < 1e6:
            options.freq *= 1e6

        self.frame = frame
        self.panel = panel

        self.state = "FREQ"
        self.freq = 0
        self.freq_step = 25e3

        self.rxpath = receive_path(options.args, options.spec, options.antenna,
                                   options.gain, options.audio_output)
	self.connect(self.rxpath)

        self._build_gui(vbox, options.no_gui)

        # set initial values

        if options.volume is not None:
            self.set_volume(options.volume)

        if not(self.set_freq(options.freq)):
            self._set_status_msg("Failed to set initial frequency")

        self.set_gain(self.rxpath.gain)               # update gui
        self.set_volume(self.rxpath.volume)           # update gui
        self.set_squelch(self.rxpath.threshold())     # update gui


    def _set_status_msg(self, msg, which=0):
        self.frame.GetStatusBar().SetStatusText(msg, which)


    def _build_gui(self, vbox, no_gui):

        def _form_set_freq(kv):
            return self.set_freq(kv['freq'])


        self.src_fft = None
        if 0 and not(no_gui):
            self.src_fft = fftsink2.fft_sink_c(self.panel,
                                               title="Data from USRP",
                                               fft_size=512,
                                               sample_rate=self.rxpath.if_rate,
                                               ref_scale=32768.0,
                                               ref_level=0,
                                               y_per_div=10,
                                               y_divs=12)
            self.connect (self.rxpath.u, self.src_fft)
            vbox.Add (self.src_fft.win, 4, wx.EXPAND)
        if 1 and not(no_gui):
            rx_fft = fftsink2.fft_sink_c(self.panel,
                                         title="Post s/w Resampling",
                                         fft_size=512,
                                         sample_rate=self.rxpath.quad_rate,
                                         ref_level=80,
                                         y_per_div=20)
            self.connect (self.rxpath.resamp, rx_fft)
            vbox.Add (rx_fft.win, 4, wx.EXPAND)

        if 1 and not(no_gui):
            post_deemph_fft = fftsink2.fft_sink_f(self.panel,
                                                  title="Post Deemph",
                                                  fft_size=512,
                                                  sample_rate=self.rxpath.audio_rate,
                                                  y_per_div=10,
                                                  ref_level=-40)
            self.connect (self.rxpath.fmrx.deemph, post_deemph_fft)
            vbox.Add (post_deemph_fft.win, 4, wx.EXPAND)

        if 0:
            post_filt_fft = fftsink2.fft_sink_f(self.panel,
                                                title="Post Filter",
                                                fft_size=512,
                                                sample_rate=audio_rate,
                                                y_per_div=10,
                                                ref_level=-40)
            self.connect (self.guts.audio_filter, post_filt)
            vbox.Add (fft_win4, 4, wx.EXPAND)

        # control area form at bottom
        self.myform = myform = form.form()

        hbox = wx.BoxSizer(wx.HORIZONTAL)
        hbox.Add((5,0), 0)
        myform['freq'] = form.float_field(
            parent=self.panel, sizer=hbox, label="Freq", weight=1,
            callback=myform.check_input_and_call(_form_set_freq,
                                                 self._set_status_msg))

        #hbox.Add((5,0), 0)
        #myform['freq_slider'] = \
        #    form.quantized_slider_field(parent=self.panel, sizer=hbox, weight=3,
        #                                range=(87.9e6, 108.1e6, 0.1e6),
        #                                callback=self.set_freq)

        hbox.Add((5,0), 0)
        vbox.Add(hbox, 0, wx.EXPAND)

        hbox = wx.BoxSizer(wx.HORIZONTAL)
        hbox.Add((5,0), 0)

        myform['volume'] = \
            form.quantized_slider_field(parent=self.panel, sizer=hbox, label="Volume",
                                        weight=3, range=self.volume_range(),
                                        callback=self.set_volume)
        hbox.Add((5,0), 0)
        myform['squelch'] = \
            form.quantized_slider_field(parent=self.panel, sizer=hbox, label="Squelch",
                                        weight=3, range=self.rxpath.squelch_range(),
                                        callback=self.set_squelch)
        g = self.rxpath.u.get_gain_range()
        hbox.Add((5,0), 0)
        myform['gain'] = \
            form.quantized_slider_field(parent=self.panel, sizer=hbox, label="Gain",
                                        weight=3, range=(g.start(), g.stop(), g.step()),
                                        callback=self.set_gain)
        hbox.Add((5,0), 0)
        vbox.Add(hbox, 0, wx.EXPAND)

        try:
            self.knob = powermate.powermate(self.frame)
            self.rot = 0
            powermate.EVT_POWERMATE_ROTATE (self.frame, self.on_rotate)
            powermate.EVT_POWERMATE_BUTTON (self.frame, self.on_button)
        except:
            print "FYI: No Powermate or Contour Knob found"


    def on_rotate (self, event):
        self.rot += event.delta
        if (self.state == "FREQ"):
            if self.rot >= 3:
                self.set_freq(self.freq + self.freq_step)
                self.rot -= 3
            elif self.rot <=-3:
                self.set_freq(self.freq - self.freq_step)
                self.rot += 3
        else:
            step = self.volume_range()[2]
            if self.rot >= 3:
                self.set_volume(self.rxpath.volume + step)
                self.rot -= 3
            elif self.rot <=-3:
                self.set_volume(self.rxpath.volume - step)
                self.rot += 3

    def on_button (self, event):
        if event.value == 0:        # button up
            return
        self.rot = 0
        if self.state == "FREQ":
            self.state = "VOL"
        else:
            self.state = "FREQ"
        self.update_status_bar ()


    def set_squelch(self, threshold_in_db):
        self.rxpath.set_squelch(threshold_in_db)
        self.myform['squelch'].set_value(self.rxpath.threshold())

    def set_volume (self, vol):
        self.rxpath.set_volume(vol)
        self.myform['volume'].set_value(self.rxpath.volume)
        self.update_status_bar ()

    def set_freq(self, target_freq):
        r = self.rxpath.set_freq(target_freq)
        if r:
            self.freq = target_freq
            self.myform['freq'].set_value(target_freq)         # update displayed value
            #self.myform['freq_slider'].set_value(target_freq)  # update displayed value
            self.update_status_bar()
            self._set_status_msg("OK", 0)
            return True

        self._set_status_msg("Failed", 0)
        return False

    def set_gain(self, gain):
        self.myform['gain'].set_value(gain)     # update displayed value
        self.rxpath.set_gain(gain)

    def update_status_bar (self):
        msg = "Volume:%r  Setting:%s" % (self.rxpath.volume, self.state)
        self._set_status_msg(msg, 1)
        if self.src_fft:
            self.src_fft.set_baseband_freq(self.freq)

    def volume_range(self):
        return (-20.0, 0.0, 0.5)


#////////////////////////////////////////////////////////////////////////
#                           Receive Path
#////////////////////////////////////////////////////////////////////////

USE_SIMPLE_SQUELCH = False

class receive_path(gr.hier_block2):
    def __init__(self, args, spec, antenna, gain, audio_output):
	gr.hier_block2.__init__(self, "receive_path",
				gr.io_signature(0, 0, 0), # Input signature
				gr.io_signature(0, 0, 0)) # Output signature

        self.u = uhd.usrp_source(device_addr=args, stream_args=uhd.stream_args('fc32'))

        # Set the subdevice spec
        if(spec):
            self.u.set_subdev_spec(spec, 0)

        # Set the antenna
        if(antenna):
            self.u.set_antenna(antenna, 0)

        self.if_rate    = 256e3
        self.quad_rate  = 64e3
        self.audio_rate = 32e3

        self.u.set_samp_rate(self.if_rate)
        dev_rate = self.u.get_samp_rate()

        # Create filter to get actual channel we want
        nfilts = 32
        chan_coeffs = filter.firdes.low_pass(nfilts,              # gain
                                             nfilts*dev_rate,     # sampling rate
                                             8e3,                 # low pass cutoff freq
                                             2e3,                 # width of trans. band
                                             filter.firdes.WIN_HANN)  # filter type
        rrate = self.quad_rate / dev_rate
        self.resamp = filter.pfb.arb_resampler_ccf(rrate, chan_coeffs, nfilts)

        if USE_SIMPLE_SQUELCH:
            self.squelch = analog.simple_squelch_cc(20)
        else:
            self.squelch = analog.standard_squelch(self.audio_rate)

        # instantiate the guts of the single channel receiver
        self.fmrx = analog.nbfm_rx(self.audio_rate, self.quad_rate)

        # audio gain / mute block
        self._audio_gain = blocks.multiply_const_ff(1.0)

        # sound card as final sink
        audio_sink = audio.sink (int(self.audio_rate), audio_output)

        # now wire it all together
        if USE_SIMPLE_SQUELCH:
            self.connect (self.u, self.resamp, self.squelch, self.fmrx,
                          self._audio_gain, audio_sink)
        else:
            self.connect (self.u, self.resamp, self.fmrx, self.squelch,
                          self._audio_gain, audio_sink)

        if gain is None:
            # if no gain was specified, use the mid-point in dB
            g = self.u.get_gain_range()
            gain = float(g.start()+g.stop())/2

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

    def _update_audio_gain(self):
        self._audio_gain.set_k(10**(self.volume/10))

    def squelch_range(self):
        r = self.squelch.squelch_range()
        #print "squelch_range: ", r
        return r

    def set_squelch(self, threshold):
        #print "SQL =", threshold
        self.squelch.set_threshold(threshold)

    def threshold(self):
        t = self.squelch.threshold()
        #print "t =", t
        return t

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

if __name__ == '__main__':
    app = stdgui2.stdapp (my_top_block, "USRP NBFM RX")
    app.MainLoop ()
