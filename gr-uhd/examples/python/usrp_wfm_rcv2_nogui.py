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
from optparse import OptionParser
import sys
import math

class wfm_rx_block (gr.top_block):

    def __init__(self):
        gr.top_block.__init__(self)

        parser=OptionParser(option_class=eng_option)
        parser.add_option("-a", "--args", type="string", default="",
                          help="UHD device address args [default=%default]")
        parser.add_option("", "--spec", type="string", default="A:0 A:0",
	                  help="Subdevice of UHD device where appropriate")
        parser.add_option("-A", "--antenna", type="string", default=None,
                          help="select Rx Antenna where appropriate")
        parser.add_option("", "--f1", type="eng_float", default=100.7e6,
                          help="set 1st station frequency to FREQ", metavar="FREQ")
        parser.add_option("", "--f2", type="eng_float", default=102.5e6,
                          help="set 2nd station freq to FREQ", metavar="FREQ")
        parser.add_option("-g", "--gain", type="eng_float", default=40,
                          help="set gain in dB (default is midpoint)")
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
        
        if abs(options.f1 - options.f2) > 5.5e6:
            print "Sorry, two stations must be within 5.5MHz of each other"
            raise SystemExit

        f = (options.f1, options.f2)
        
        self.vol = .1
        self.state = "FREQ"

        self.fm_freq_min = options.freq_min
        self.fm_freq_max = options.freq_max

        # build graph
        stream_args = uhd.stream_args('fc32', channels=range(2))
        self.u = uhd.usrp_source(device_addr=options.args, stream_args=stream_args)

        # Set front end channel mapping
        self.u.set_subdev_spec(options.spec)

        usrp_rate  = 320e3
        demod_rate = 320e3
        audio_rate = 32e3
        audio_decim = int(demod_rate / audio_rate)

        self.u.set_samp_rate(usrp_rate)
        dev_rate = self.u.get_samp_rate()

        # Make sure dboard can suppor the required frequencies
        frange = self.u.get_freq_range()
        if(frange.start() > self.fm_freq_max or frange.stop() <  self.fm_freq_min):
            sys.stderr.write("Radio does not support required frequency range.\n")
            sys.exit(1)

        # sound card as final sink
        self.audio_sink = audio.sink(int(audio_rate), options.audio_output)

        # taps for channel filter
        nfilts = 32
        chan_coeffs = optfir.low_pass (nfilts,           # gain
                                       nfilts*usrp_rate, # sampling rate
                                       80e3,             # passband cutoff
                                       115e3,            # stopband cutoff
                                       0.1,              # passband ripple
                                       60)               # stopband attenuation
        rrate = usrp_rate / dev_rate

        # set front end PLL to middle frequency
        mid_freq = (f[0] + f[1]) / 2.0

        if options.gain is None:
            # if no gain was specified, use the mid-point in dB
            g = self.u.get_gain_range()
            options.gain = float(g.start()+g.stop())/2.0

        for n in range(2):
           chan_filt = blks2.pfb_arb_resampler_ccf(rrate, chan_coeffs, nfilts)
           guts = blks2.wfm_rcv (demod_rate, audio_decim)
           volume_control = gr.multiply_const_ff(self.vol)

           #self.connect((self.di, n), chan_filt)
           self.connect((self.u, n), chan_filt)
           self.connect(chan_filt, guts, volume_control)
           self.connect(volume_control, (self.audio_sink, n))

           # Test the the requested frequencies are in range
           if(f[n] < self.fm_freq_min or f[n] > self.fm_freq_max):
              sys.stderr.write("Requested frequency is outside of required frequency range.\n")
              sys.exit(1)

           # Tune each channel by setting the RF freq to mid_freq and the
           # DDC freq to f[n].
           tr = uhd.tune_request(f[n], rf_freq=mid_freq,
                                 rf_freq_policy=uhd.tune_request.POLICY_MANUAL)
           self.u.set_center_freq(tr, n)
           
           # Set gain for each channel
           self.set_gain(options.gain, n)

           # Set the antenna
           if(options.antenna):
               self.u.set_antenna(options.antenna, n)

    def set_vol (self, vol):
        self.vol = vol
        self.volume_control.set_k(self.vol)


    def set_gain(self, gain, n):
        self.u.set_gain(gain, n)

if __name__ == '__main__':
    tb = wfm_rx_block()
    try:
        tb.run()
    except KeyboardInterrupt:
        pass
