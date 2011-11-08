#!/usr/bin/env python
#
# Copyright 2011 Free Software Foundation, Inc.
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

from gnuradio import eng_notation
from gnuradio import gr
from gnuradio import uhd
from gnuradio import window
from gnuradio.eng_option import eng_option
from gnuradio.gr import firdes
from optparse import OptionParser

class uhd_burst_detector(gr.top_block):
    def __init__(self, uhd_address, options):

        gr.top_block.__init__(self)

        self.uhd_addr = uhd_address
        self.freq = options.freq
        self.samp_rate = options.samp_rate
        self.gain = options.gain
        self.threshold = options.threshold
        self.trigger = options.trigger
        
        self.uhd_src = uhd.single_usrp_source(
            device_addr=self.uhd_addr,
            stream_args=uhd.stream_args('fc32'))
        
        self.uhd_src.set_samp_rate(self.samp_rate)
        self.uhd_src.set_center_freq(self.freq, 0)
        self.uhd_src.set_gain(self.gain, 0)

        taps = firdes.low_pass_2(1, 1, 0.4, 0.1, 60)
        self.chanfilt = gr.fir_filter_ccc(10, taps)
        self.tagger = gr.burst_tagger(gr.sizeof_gr_complex)

        # Dummy signaler to collect a burst on known periods
        data = 1000*[0,] + 1000*[1,]
        self.signal = gr.vector_source_s(data, True)

        # Energy detector to get signal burst
        ## use squelch to detect energy
        self.det  = gr.simple_squelch_cc(self.threshold, 0.01)
        ## convert to mag squared (float)
        self.c2m = gr.complex_to_mag_squared()
        ## average to debounce
        self.avg = gr.single_pole_iir_filter_ff(0.01)
        ## rescale signal for conversion to short
        self.scale = gr.multiply_const_ff(2**16)
        ## signal input uses shorts
        self.f2s = gr.float_to_short()

        # Use file sink burst tagger to capture bursts
        self.fsnk = gr.tagged_file_sink(gr.sizeof_gr_complex, self.samp_rate)
        

        ##################################################
        # Connections
        ##################################################
        self.connect((self.uhd_src, 0), (self.tagger, 0))
        self.connect((self.tagger, 0), (self.fsnk, 0))

        if self.trigger:
            # Connect a dummy signaler to the burst tagger
            self.connect((self.signal, 0), (self.tagger, 1))

        else:
            # Connect an energy detector signaler to the burst tagger
            self.connect(self.uhd_src, self.det)
            self.connect(self.det, self.c2m, self.avg, self.scale, self.f2s)
            self.connect(self.f2s, (self.tagger, 1))
        
    def set_samp_rate(self, samp_rate):
        self.samp_rate = samp_rate
        self.uhd_src_0.set_samp_rate(self.samp_rate)
            
if __name__ == '__main__':
    parser = OptionParser(option_class=eng_option, usage="%prog: [options]")
    parser.add_option("-a", "--address", type="string", default="addr=192.168.10.2",
                      help="select address of the device [default=%default]")
    #parser.add_option("-A", "--antenna", default=None,
    #                  help="select Rx Antenna (only on RFX-series boards)")
    parser.add_option("-f", "--freq", type="eng_float", default=450e6,
                      help="set frequency to FREQ", metavar="FREQ")
    parser.add_option("-g", "--gain", type="eng_float", default=0,
                      help="set gain in dB [default=%default]")
    parser.add_option("-R", "--samp-rate", type="eng_float", default=200000,
                      help="set USRP sample rate [default=%default]")
    parser.add_option("-t", "--threshold", type="float", default=-60,
                      help="Set the detection power threshold (dBm) [default=%default")
    parser.add_option("-T", "--trigger", action="store_true", default=False,
                      help="Use internal trigger instead of detector [default=%default]")
    (options, args) = parser.parse_args()
    
    uhd_addr = options.address

    tb = uhd_burst_detector(uhd_addr, options)
    tb.run()
