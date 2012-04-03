#!/usr/bin/env python
#
# Copyright 2005-2007,2011 Free Software Foundation, Inc.
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

from gnuradio import gr, optfir, audio, blks2, uhd
from gnuradio.eng_option import eng_option
from gnuradio.wxgui import slider, powermate
from gnuradio.wxgui import stdgui2, fftsink2, form, scopesink2
from optparse import OptionParser
import sys
import wx

class wfm_rx_block (stdgui2.std_top_block):
    def __init__(self,frame,panel,vbox,argv):
        stdgui2.std_top_block.__init__ (self,frame,panel,vbox,argv)

        parser=OptionParser(option_class=eng_option)
        parser.add_option("-a", "--args", type="string", default="",
                          help="UHD device address args [default=%default]")
        parser.add_option("", "--spec", type="string", default=None,
	                  help="Subdevice of UHD device where appropriate")
        parser.add_option("-A", "--antenna", type="string", default=None,
                          help="select Rx Antenna where appropriate")
        parser.add_option("-f", "--freq", type="eng_float", default=100.1e6,
                          help="set frequency to FREQ", metavar="FREQ")
        parser.add_option("-g", "--gain", type="eng_float", default=65,
                          help="set gain in dB (default is midpoint)")
        parser.add_option("-s", "--squelch", type="eng_float", default=0,
                          help="set squelch level (default is 0)")
        parser.add_option("-V", "--volume", type="eng_float", default=None,
                          help="set volume (default is midpoint)")
        parser.add_option("-O", "--audio-output", type="string", default="",
                          help="pcm device name.  E.g., hw:0,0 or surround51 or /dev/dsp")
        parser.add_option("", "--freq-min", type="eng_float", default=87.9e6,
                          help="Set a minimum frequency [default=%default]")
        parser.add_option("", "--freq-max", type="eng_float", default=108.1e6,
                          help="Set a maximum frequency [default=%default]")


        (options, args) = parser.parse_args()
        if len(args) != 0:
            parser.print_help()
            sys.exit(1)
        
        self.frame = frame
        self.panel = panel
        
        self.vol = 0
        self.state = "FREQ"
        self.freq = 0

        self.fm_freq_min = options.freq_min
        self.fm_freq_max = options.freq_max

        # build graph
        self.u = uhd.usrp_source(device_addr=options.args, stream_args=uhd.stream_args('fc32'))

        # Set the subdevice spec
        if(options.spec):
            self.u.set_subdev_spec(options.spec, 0)

        # Set the antenna
        if(options.antenna):
            self.u.set_antenna(options.antenna, 0)

        usrp_rate  = 320e3
        demod_rate = 320e3
        audio_rate = 48e3
        audio_decim = 10

        self.u.set_samp_rate(usrp_rate)
        dev_rate = self.u.get_samp_rate()

        nfilts = 32
        chan_coeffs = gr.firdes.low_pass_2(10*nfilts,           # gain
                                           nfilts*usrp_rate, # sampling rate
                                           90e3,             # passband cutoff
                                           30e3,             # transition bw
                                           70)               # stopband attenuation
        rrate = usrp_rate / dev_rate
        self.chan_filt = blks2.pfb_arb_resampler_ccf(rrate, chan_coeffs, nfilts)

        self.guts = blks2.wfm_rcv_fmdet (demod_rate, audio_decim)

        chan_rate = audio_rate / (demod_rate/audio_decim)
        self.rchan_filt = blks2.pfb_arb_resampler_fff(chan_rate)
        self.lchan_filt = blks2.pfb_arb_resampler_fff(chan_rate)

        # FIXME rework {add,multiply}_const_* to handle multiple streams
        self.volume_control_l = gr.multiply_const_ff(self.vol)
        self.volume_control_r = gr.multiply_const_ff(self.vol)

        # sound card as final sink
        self.audio_sink = audio.sink (int (audio_rate),
                                      options.audio_output,
                                      False)   # ok_to_block
        
        # now wire it all together
        self.connect (self.u, self.chan_filt, self.guts)
        self.connect((self.guts, 0), self.lchan_filt,
                     self.volume_control_l, (self.audio_sink,0))
        self.connect((self.guts, 1), self.rchan_filt,
                     self.volume_control_r, (self.audio_sink,1))

        try:
          self.guts.stereo_carrier_pll_recovery.squelch_enable(True)
        except:
          print "FYI: This implementation of the stereo_carrier_pll_recovery has no squelch implementation yet"


        self._build_gui(vbox, usrp_rate, demod_rate, audio_rate)

        if options.gain is None:
            # if no gain was specified, use the mid-point in dB
            g = self.u.get_gain_range()
            options.gain = float(g.start()+g.stop())/2.0

        if options.volume is None:
            g = self.volume_range()
            options.volume = float(g[0]+g[1])/2

        if abs(options.freq) < 1e6:
            options.freq *= 1e6

        frange = self.u.get_freq_range()
        if(frange.start() > self.fm_freq_max or frange.stop() <  self.fm_freq_min):
            sys.stderr.write("Radio does not support required frequency range.\n")
            sys.exit(1)
        if(options.freq < self.fm_freq_min or options.freq > self.fm_freq_max):
            sys.stderr.write("Requested frequency is outside of required frequency range.\n")
            sys.exit(1)

        # set initial values
        self.set_gain(options.gain)
        self.set_vol(options.volume)
        try:
          self.guts.stereo_carrier_pll_recovery.set_lock_threshold(options.squelch)
        except:
          print "FYI: This implementation of the stereo_carrier_pll_recovery has no squelch implementation yet"

        if not(self.set_freq(options.freq)):
            self._set_status_msg("Failed to set initial frequency")

    def _set_status_msg(self, msg, which=0):
        self.frame.GetStatusBar().SetStatusText(msg, which)


    def _build_gui(self, vbox, usrp_rate, demod_rate, audio_rate):

        def _form_set_freq(kv):
            return self.set_freq(kv['freq'])


        if 1:
            self.src_fft = fftsink2.fft_sink_c(self.panel, title="Data from USRP",
                                               fft_size=512, sample_rate=usrp_rate,
					       ref_scale=32768.0, ref_level=0, y_divs=12)
            self.connect (self.u, self.src_fft)
            vbox.Add (self.src_fft.win, 4, wx.EXPAND)

        if 1:
            post_fm_demod_fft = fftsink2.fft_sink_f(self.panel, title="Post FM Demod",
                                                    fft_size=512, sample_rate=demod_rate,
                                                    y_per_div=10, ref_level=0)
            self.connect (self.guts.fm_demod, post_fm_demod_fft)
            vbox.Add (post_fm_demod_fft.win, 4, wx.EXPAND)

        if 0:
            post_stereo_carrier_generator_fft = fftsink2.fft_sink_c (self.panel, title="Post Stereo_carrier_generator",
                                                  fft_size=512, sample_rate=audio_rate,
                                                  y_per_div=10, ref_level=0)
            self.connect (self.guts.stereo_carrier_generator, post_stereo_carrier_generator_fft)
            vbox.Add (post_stereo_carrier_generator_fft.win, 4, wx.EXPAND)

        if 0:
            post_deemphasis_left = fftsink2.fft_sink_f (self.panel, title="Post_Deemphasis_Left",
                                                  fft_size=512, sample_rate=audio_rate,
                                                  y_per_div=10, ref_level=0)
            self.connect (self.guts.deemph_Left, post_deemphasis_left)
            vbox.Add (post_deemphasis_left.win, 4, wx.EXPAND)

        if 0:
            post_deemphasis_right = fftsink2.fft_sink_f(self.panel, title="Post_Deemphasis_Right",
                                                  fft_size=512, sample_rate=audio_rate,
                                                  y_per_div=10, ref_level=-20)
            self.connect (self.guts.deemph_Left, post_deemphasis_right)
            vbox.Add (post_deemphasis_right.win, 4, wx.EXPAND)


        if 0:
            LmR_fft = fftsink2.fft_sink_f(self.panel, title="LmR", 
                                          fft_size=512, sample_rate=audio_rate,
                                          y_per_div=10, ref_level=-20)
            self.connect (self.guts.LmR_real,LmR_fft)
            vbox.Add (LmR_fft.win, 4, wx.EXPAND)

        if 0:
            self.scope = scopesink2.scope_sink_f(self.panel, sample_rate=demod_rate)
            self.connect (self.guts.fm_demod,self.scope)
            vbox.Add (self.scope.win,4,wx.EXPAND)
        
        # control area form at bottom
        self.myform = myform = form.form()

        hbox = wx.BoxSizer(wx.HORIZONTAL)
        hbox.Add((5,0), 0)
        myform['freq'] = form.float_field(
            parent=self.panel, sizer=hbox, label="Freq", weight=1,
            callback=myform.check_input_and_call(_form_set_freq, self._set_status_msg))

        hbox.Add((5,0), 0)
        myform['freq_slider'] = \
            form.quantized_slider_field(parent=self.panel, sizer=hbox, weight=3,
                                        range=(self.fm_freq_min, self.fm_freq_max, 0.1e6),
                                        callback=self.set_freq)
        hbox.Add((5,0), 0)
        vbox.Add(hbox, 0, wx.EXPAND)

        hbox = wx.BoxSizer(wx.HORIZONTAL)
        hbox.Add((5,0), 0)

        myform['volume'] = \
            form.quantized_slider_field(parent=self.panel, sizer=hbox, label="Volume",
                                        weight=3, range=self.volume_range(),
                                        callback=self.set_vol)
        hbox.Add((5,0), 1)

        g = self.u.get_gain_range()        
        myform['gain'] = \
            form.quantized_slider_field(parent=self.panel, sizer=hbox, label="Gain",
                                        weight=3, range=(g.start(), g.stop(), g.step()),
                                        callback=self.set_gain)
        hbox.Add((5,0), 0)

        myform['sqlch_thrsh'] = \
            form.quantized_slider_field(parent=self.panel, sizer=hbox, label="Stereo Squelch Threshold",
                                        weight=3, range=(0.0,1.0,0.01),
                                        callback=self.set_squelch)
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
                self.set_freq(self.freq + .1e6)
                self.rot -= 3
            elif self.rot <=-3:
                self.set_freq(self.freq - .1e6)
                self.rot += 3
        else:
            step = self.volume_range()[2]
            if self.rot >= 3:
                self.set_vol(self.vol + step)
                self.rot -= 3
            elif self.rot <=-3:
                self.set_vol(self.vol - step)
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
        

    def set_vol (self, vol):
        g = self.volume_range()
        self.vol = max(g[0], min(g[1], vol))
        self.volume_control_l.set_k(10**(self.vol/10))
        self.volume_control_r.set_k(10**(self.vol/10))
        self.myform['volume'].set_value(self.vol)
        self.update_status_bar ()

    def set_squelch(self,squelch_threshold):
        try:
          self.guts.stereo_carrier_pll_recovery.set_lock_threshold(squelch_threshold);
        except:
          print "FYI: This implementation of the stereo_carrier_pll_recovery has no squelch implementation yet"
                                        

    def set_freq(self, target_freq):
        """
        Set the center frequency we're interested in.

        @param target_freq: frequency in Hz
        @rypte: bool
        """
        
        r = self.u.set_center_freq(target_freq)

        if r:
            self.freq = target_freq
            self.myform['freq'].set_value(target_freq)         # update displayed value
            self.myform['freq_slider'].set_value(target_freq)  # update displayed value
            self.update_status_bar()
            self._set_status_msg("OK", 0)
            return True

        self._set_status_msg("Failed", 0)
        return False
        
    def set_gain(self, gain):
        self.myform['gain'].set_value(gain)     # update displayed value
        self.u.set_gain(gain)

    def update_status_bar (self):
        msg = "Volume:%r  Setting:%s" % (self.vol, self.state)
        self._set_status_msg(msg, 1)
        self.src_fft.set_baseband_freq(self.freq)

    def volume_range(self):
        return (-20.0, 0.0, 0.5)
        

if __name__ == '__main__':
    app = stdgui2.stdapp (wfm_rx_block, "USRP WFM RX")
    app.MainLoop ()
