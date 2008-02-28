#!/usr/bin/env python
#
# Copyright 2005,2006,2007 Free Software Foundation, Inc.
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

def pick_subdevice(u):
    """
    The user didn't specify a subdevice on the command line.
    Try for one of these, in order: BASIC_RX,TV_RX, BASIC_RX, whatever is on side A.

    @return a subdev_spec
    """
    return usrp.pick_subdev(u, (usrp_dbid.BASIC_RX,
                                usrp_dbid.LF_RX,
                                usrp_dbid.TV_RX,
                                usrp_dbid.TV_RX_REV_2,
				usrp_dbid.TV_RX_REV_3))


class wfm_rx_block (stdgui2.std_top_block):
    def __init__(self,frame,panel,vbox,argv):
        stdgui2.std_top_block.__init__ (self,frame,panel,vbox,argv)

        parser=OptionParser(option_class=eng_option)
        parser.add_option("-R", "--rx-subdev-spec", type="subdev", default=None,
                          help="select USRP Rx side A or B (default=A)")
        parser.add_option("-f", "--freq", type="eng_float", default=1008.0e3,
                          help="set frequency to FREQ", metavar="FREQ")
        parser.add_option("-I", "--use-if-freq", action="store_true", default=False,
                          help="use intermediate freq (compensates DC problems in quadrature boards)" )
        parser.add_option("-g", "--gain", type="eng_float", default=None,
                          help="set gain in dB (default is maximum)")
        parser.add_option("-V", "--volume", type="eng_float", default=None,
                          help="set volume (default is midpoint)")
        parser.add_option("-O", "--audio-output", type="string", default="",
                          help="pcm device name.  E.g., hw:0,0 or surround51 or /dev/dsp")

        (options, args) = parser.parse_args()
        if len(args) != 0:
            parser.print_help()
            sys.exit(1)
        
        self.frame = frame
        self.panel = panel
        self.use_IF=options.use_if_freq
        if self.use_IF:
          self.IF_freq=64000.0 
        else:
          self.IF_freq=0.0
        
        self.vol = 0
        self.state = "FREQ"
        self.freq = 0

        # build graph

        #TODO: add an AGC after the channel filter and before the AM_demod
        
        self.u = usrp.source_c()                    # usrp is data source

        adc_rate = self.u.adc_rate()                # 64 MS/s
        usrp_decim = 250
        self.u.set_decim_rate(usrp_decim)
        usrp_rate = adc_rate / usrp_decim           # 256 kS/s
        chanfilt_decim = 4
        demod_rate = usrp_rate / chanfilt_decim     # 64 kHz
        audio_decimation = 2
        audio_rate = demod_rate / audio_decimation  # 32 kHz

        if options.rx_subdev_spec is None:
            options.rx_subdev_spec = pick_subdevice(self.u)

        self.u.set_mux(usrp.determine_rx_mux_value(self.u, options.rx_subdev_spec))
        self.subdev = usrp.selected_subdev(self.u, options.rx_subdev_spec)
        print "Using RX d'board %s" % (self.subdev.side_and_name(),)


        chan_filt_coeffs = optfir.low_pass (1,           # gain
                                            usrp_rate,   # sampling rate
                                            8e3,        # passband cutoff
                                            12e3,       # stopband cutoff
                                            1.0,         # passband ripple
                                            60)          # stopband attenuation
        #print len(chan_filt_coeffs)
        self.chan_filt = gr.fir_filter_ccf (chanfilt_decim, chan_filt_coeffs)
        if self.use_IF:
          # Turn If to baseband and filter.
          self.chan_filt = gr.freq_xlating_fir_filter_ccf (chanfilt_decim, chan_filt_coeffs, self.IF_freq, usrp_rate)
        else:
          self.chan_filt = gr.fir_filter_ccf (chanfilt_decim, chan_filt_coeffs)
        self.am_demod = gr.complex_to_mag()

        self.volume_control = gr.multiply_const_ff(self.vol)

        audio_filt_coeffs = optfir.low_pass (1,           # gain
                                            demod_rate,   # sampling rate
                                            8e3,        # passband cutoff
                                            10e3,       # stopband cutoff
                                            0.1,         # passband ripple
                                            60)          # stopband attenuation
        self.audio_filt=gr.fir_filter_fff(audio_decimation,audio_filt_coeffs)
        # sound card as final sink
        audio_sink = audio.sink (int (audio_rate),
                                 options.audio_output,
                                 False)  # ok_to_block
        
        # now wire it all together
        self.connect (self.u, self.chan_filt, self.am_demod, self.audio_filt, self.volume_control, audio_sink)

        self._build_gui(vbox, usrp_rate, demod_rate, audio_rate)

        if options.gain is None:
            g = self.subdev.gain_range()
            if True:
              # if no gain was specified, use the maximum gain available 
              # (usefull for Basic_RX which is relatively deaf and the most probable board to be used for AM)
              # TODO: check db type to decide on default gain.
              options.gain = float(g[1])
            else:
              # if no gain was specified, use the mid-point in dB
              options.gain = float(g[0]+g[1])/2


        if options.volume is None:
            g = self.volume_range()
            options.volume = float(g[0]*3+g[1])/4
            
        if abs(options.freq) < 1e3:
            options.freq *= 1e3

        # set initial values

        self.set_gain(options.gain)
        self.set_vol(options.volume)
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
					       ref_scale=32768.0, ref_level=0.0, y_divs=12)
            self.connect (self.u, self.src_fft)
            vbox.Add (self.src_fft.win, 4, wx.EXPAND)

        if 0:
            self.post_filt_fft = fftsink2.fft_sink_c(self.panel, title="Post Channel filter",
                                               fft_size=512, sample_rate=demod_rate)
            self.connect (self.chan_filt, self.post_filt_fft)
            vbox.Add (self.post_filt_fft.win, 4, wx.EXPAND)

        if 0:
            post_demod_fft = fftsink2.fft_sink_f(self.panel, title="Post Demod", 
                                                fft_size=1024, sample_rate=demod_rate,
                                                y_per_div=10, ref_level=0)
            self.connect (self.am_demod, post_demod_fft)
            vbox.Add (post_demod_fft.win, 4, wx.EXPAND)

        if 1:
            audio_fft = fftsink2.fft_sink_f(self.panel, title="Audio",
                                                  fft_size=512, sample_rate=audio_rate,
                                                  y_per_div=10, ref_level=20)
            self.connect (self.audio_filt, audio_fft)
            vbox.Add (audio_fft.win, 4, wx.EXPAND)

        
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
                                        range=(520.0e3, 1611.0e3, 1.0e3),
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

        myform['gain'] = \
            form.quantized_slider_field(parent=self.panel, sizer=hbox, label="Gain",
                                        weight=3, range=self.subdev.gain_range(),
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
        self.volume_control.set_k(10**(self.vol/10))
        self.myform['volume'].set_value(self.vol)
        self.update_status_bar ()
                                        
    def set_freq(self, target_freq):
        """
        Set the center frequency we're interested in.

        @param target_freq: frequency in Hz
        @rypte: bool

        Tuning is a two step process.  First we ask the front-end to
        tune as close to the desired frequency as it can.  Then we use
        the result of that operation and our target_frequency to
        determine the value for the digital down converter.
        """
        r = usrp.tune(self.u, 0, self.subdev, target_freq  + self.IF_freq)
        #TODO: check if db is inverting the spectrum or not to decide if we should do + self.IF_freq  or - self.IF_freq
        
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
        self.subdev.set_gain(gain)

    def update_status_bar (self):
        msg = "Volume:%r  Setting:%s" % (self.vol, self.state)
        self._set_status_msg(msg, 1)
        try:
          self.src_fft.set_baseband_freq(self.freq)
        except:
          None
          
    def volume_range(self):
        return (-40.0, 0.0, 0.5)
        

if __name__ == '__main__':
    app = stdgui2.stdapp (wfm_rx_block, "USRP Broadcast AM MW RX")
    app.MainLoop ()
