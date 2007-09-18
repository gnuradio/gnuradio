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
from optparse import OptionParser
from usrpm import usrp_dbid
import sys
import math

def pick_subdevice(u):
    """
    The user didn't specify a subdevice on the command line.
    Try for one of these, in order: TV_RX, BASIC_RX, whatever is on side A.

    @return a subdev_spec
    """
    return usrp.pick_subdev(u, (usrp_dbid.TV_RX,
                                usrp_dbid.TV_RX_REV_2,
                                usrp_dbid.BASIC_RX))


class wfm_rx_block (gr.top_block):

    def __init__(self):
        gr.top_block.__init__(self)

        parser=OptionParser(option_class=eng_option)
        parser.add_option("-R", "--rx-subdev-spec", type="subdev", default=None,
                          help="select USRP Rx side A or B (default=A)")
        parser.add_option("", "--f1", type="eng_float", default=100.7e6,
                          help="set 1st station frequency to FREQ", metavar="FREQ")
        parser.add_option("", "--f2", type="eng_float", default=102.5e6,
                          help="set 2nd station freq to FREQ", metavar="FREQ")
        parser.add_option("-g", "--gain", type="eng_float", default=40,
                          help="set gain in dB (default is midpoint)")
        parser.add_option("-O", "--audio-output", type="string", default="",
                          help="pcm device name.  E.g., hw:0,0 or surround51 or /dev/dsp")

        (options, args) = parser.parse_args()
        if len(args) != 0:
            parser.print_help()
            sys.exit(1)
        
        if abs(options.f1) < 1e6:
            options.f1 *= 1e6

        if abs(options.f2) < 1e6:
            options.f2 *= 1e6

        if abs(options.f1 - options.f2) > 5.5e6:
            print "Sorry, two stations must be within 5.5MHz of each other"
            raise SystemExit

        f = (options.f1, options.f2)
        
        self.vol = .1
        self.state = "FREQ"

        # build graph
        
        self.u = usrp.source_c(0, nchan=2)          # usrp is data source

        adc_rate = self.u.adc_rate()                # 64 MS/s
        usrp_decim = 200
        self.u.set_decim_rate(usrp_decim)
        usrp_rate = adc_rate / usrp_decim           # 320 kS/s
        chanfilt_decim = 1
        demod_rate = usrp_rate / chanfilt_decim
        audio_decimation = 10
        audio_rate = demod_rate / audio_decimation  # 32 kHz


        if options.rx_subdev_spec is None:
            options.rx_subdev_spec = pick_subdevice(self.u)

        mv = usrp.determine_rx_mux_value(self.u, options.rx_subdev_spec)
        mv |= (mv << 8) & 0xff00   # both DDC inputs setup same way
        self.u.set_mux(mv)
        self.subdev = usrp.selected_subdev(self.u, options.rx_subdev_spec)
        print "Using RX d'board %s" % (self.subdev.side_and_name(),)


        # deinterleave two channels from FPGA
        di = gr.deinterleave(gr.sizeof_gr_complex)
        
        # wire up the head of the chain
        self.connect(self.u, di)
        
        # sound card as final sink
        audio_sink = audio.sink(int(audio_rate), options.audio_output)

        # taps for channel filter
        chan_filt_coeffs = optfir.low_pass (1,           # gain
                                            usrp_rate,   # sampling rate
                                            80e3,        # passband cutoff
                                            115e3,       # stopband cutoff
                                            0.1,         # passband ripple
                                            60)          # stopband attenuation
        #print len(chan_filt_coeffs)

        mid_freq = (f[0] + f[1]) / 2
        # set front end PLL to middle frequency
        ok, baseband_freq = self.subdev.set_freq(mid_freq)

        for n in range(2):
            chan_filt = gr.fir_filter_ccf (chanfilt_decim, chan_filt_coeffs)
            guts = blks2.wfm_rcv (demod_rate, audio_decimation)
            volume_control = gr.multiply_const_ff(self.vol)
            self.connect((di, n), chan_filt)
            self.connect(chan_filt, guts, volume_control)
            self.connect(volume_control, (audio_sink, n))
            dxc_freq, inverted = usrp.calc_dxc_freq(f[n], baseband_freq,
                                                    self.u.converter_rate())
            self.u.set_rx_freq(n, dxc_freq)
        

        if options.gain is None:
            # if no gain was specified, use the mid-point in dB
            g = self.subdev.gain_range()
            options.gain = float(g[0]+g[1])/2


        # set initial values
        self.set_gain(options.gain)


    def set_vol (self, vol):
        self.vol = vol
        self.volume_control.set_k(self.vol)


    def set_gain(self, gain):
        self.subdev.set_gain(gain)

    def __del__(self):
	# Avoid weak-reference error
	del self.subdev
    
if __name__ == '__main__':
    tb = wfm_rx_block()
    try:
        tb.run()
    except KeyboardInterrupt:
        pass
