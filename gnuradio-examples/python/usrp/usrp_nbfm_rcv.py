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

from gnuradio import gr, gru, eng_notation, optfir
from gnuradio import audio
from gnuradio import usrp
from gnuradio import blks2
from gnuradio.eng_option import eng_option
from gnuradio.wxgui import slider, powermate
from gnuradio.wxgui import stdgui2, fftsink2, form
from optparse import OptionParser
from usrpm import usrp_dbid
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
        parser.add_option("-R", "--rx-subdev-spec", type="subdev", default=None,
                          help="select USRP Rx side A or B (default=A)")
        parser.add_option("-f", "--freq", type="eng_float", default=146.585e6,
                          help="set frequency to FREQ", metavar="FREQ")
        parser.add_option("-g", "--gain", type="eng_float", default=None,
                          help="set gain in dB (default is midpoint)")
        parser.add_option("-V", "--volume", type="eng_float", default=None,
                          help="set volume (default is midpoint)")
        parser.add_option("-O", "--audio-output", type="string", default="",
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

        self.rxpath = receive_path(options.rx_subdev_spec, options.gain, options.audio_output)
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
        if 1 and not(no_gui):
            self.src_fft = fftsink2.fft_sink_c(self.panel, title="Data from USRP",
                                               fft_size=512, sample_rate=self.rxpath.if_rate,
                                               ref_scale=32768.0, ref_level=0, y_per_div=10, y_divs=12)
            self.connect (self.rxpath.u, self.src_fft)
            vbox.Add (self.src_fft.win, 4, wx.EXPAND)
        if 1 and not(no_gui):
            rx_fft = fftsink2.fft_sink_c(self.panel, title="Post s/w DDC",
                                         fft_size=512, sample_rate=self.rxpath.quad_rate,
                                         ref_level=80, y_per_div=20)
            self.connect (self.rxpath.ddc, rx_fft)
            vbox.Add (rx_fft.win, 4, wx.EXPAND)
        
        if 1 and not(no_gui):
            post_deemph_fft = fftsink2.fft_sink_f(self.panel, title="Post Deemph",
                                                  fft_size=512, sample_rate=self.rxpath.audio_rate,
                                                  y_per_div=10, ref_level=-40)
            self.connect (self.rxpath.fmrx.deemph, post_deemph_fft)
            vbox.Add (post_deemph_fft.win, 4, wx.EXPAND)

        if 0:
            post_filt_fft = fftsink2.fft_sink_f(self.panel, title="Post Filter", 
                                                fft_size=512, sample_rate=audio_rate,
                                                y_per_div=10, ref_level=-40)
            self.connect (self.guts.audio_filter, post_filt)
            vbox.Add (fft_win4, 4, wx.EXPAND)

        
        # control area form at bottom
        self.myform = myform = form.form()

        hbox = wx.BoxSizer(wx.HORIZONTAL)
        hbox.Add((5,0), 0)
        myform['freq'] = form.float_field(
            parent=self.panel, sizer=hbox, label="Freq", weight=1,
            callback=myform.check_input_and_call(_form_set_freq, self._set_status_msg))

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
        hbox.Add((5,0), 0)
        myform['gain'] = \
            form.quantized_slider_field(parent=self.panel, sizer=hbox, label="Gain",
                                        weight=3, range=self.rxpath.subdev.gain_range(),
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
    def __init__(self, subdev_spec, gain, audio_output):
	gr.hier_block2.__init__(self, "receive_path",
				gr.io_signature(0, 0, 0), # Input signature
				gr.io_signature(0, 0, 0)) # Output signature

        self.u = usrp.source_c ()
        adc_rate = self.u.adc_rate()

        self.if_rate = 256e3                              # 256 kS/s
        usrp_decim = int(adc_rate // self.if_rate)
        if_decim = 4
        self.u.set_decim_rate(usrp_decim)
        self.quad_rate = self.if_rate // if_decim         #  64 kS/s
        audio_decim = 2
        self.audio_rate = self.quad_rate // audio_decim   #  32 kS/s


        if subdev_spec is None:
            subdev_spec = usrp.pick_rx_subdevice(self.u)
        self.subdev = usrp.selected_subdev(self.u, subdev_spec)
        print "Using RX d'board %s" % (self.subdev.side_and_name(),)

        self.u.set_mux(usrp.determine_rx_mux_value(self.u, subdev_spec))

        # Create filter to get actual channel we want
        chan_coeffs = gr.firdes.low_pass (1.0,                # gain
                                          self.if_rate,       # sampling rate
                                          8e3,               # low pass cutoff freq
                                          2e3,                # width of trans. band
                                          gr.firdes.WIN_HANN) # filter type 

        print "len(rx_chan_coeffs) =", len(chan_coeffs)

        # Decimating Channel filter with frequency translation
        # complex in and out, float taps
        self.ddc = gr.freq_xlating_fir_filter_ccf(if_decim,       # decimation rate
                                                  chan_coeffs,    # taps
                                                  0,              # frequency translation amount
                                                  self.if_rate)   # input sample rate

        if USE_SIMPLE_SQUELCH:
            self.squelch = gr.simple_squelch_cc(20)
        else:
            self.squelch = blks2.standard_squelch(self.audio_rate)

        # instantiate the guts of the single channel receiver
        self.fmrx = blks2.nbfm_rx(self.audio_rate, self.quad_rate)

        # audio gain / mute block
        self._audio_gain = gr.multiply_const_ff(1.0)

        # sound card as final sink
        audio_sink = audio.sink (int(self.audio_rate), audio_output)
        
        # now wire it all together
        if USE_SIMPLE_SQUELCH:
            self.connect (self.u, self.ddc, self.squelch, self.fmrx,
                          self._audio_gain, audio_sink)
        else:
            self.connect (self.u, self.ddc, self.fmrx, self.squelch,
                          self._audio_gain, audio_sink)

        if gain is None:
            # if no gain was specified, use the mid-point in dB
            g = self.subdev.gain_range()
            gain = float(g[0]+g[1])/2

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

        @param target_freq: frequency in Hz
        @rypte: bool

        Tuning is a two step process.  First we ask the front-end to
        tune as close to the desired frequency as it can.  Then we use
        the result of that operation and our target_frequency to
        determine the value for the digital down converter in the
        FPGA.  Finally, we feed any residual_freq to the s/w freq
        translator.
        """

        r = usrp.tune(self.u, 0, self.subdev, target_freq)
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

if __name__ == '__main__':
    app = stdgui2.stdapp (my_top_block, "USRP NBFM RX")
    app.MainLoop ()
