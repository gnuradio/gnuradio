#!/usr/bin/env python
#
# Copyright 2012,2013 Free Software Foundation, Inc.
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

from __future__ import unicode_literals
from gnuradio import eng_notation
from gnuradio import gr
from gnuradio import filter, analog, blocks
from gnuradio import uhd
from gnuradio.fft import window
from gnuradio.eng_arg import eng_float
from gnuradio.filter import firdes
from argparse import ArgumentParser

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
        self.chanfilt = filter.fir_filter_ccc(10, taps)
        self.tagger = blocks.burst_tagger(gr.sizeof_gr_complex)

        # Dummy signaler to collect a burst on known periods
        data = 1000*[0,] + 1000*[1,]
        self.signal = blocks.vector_source_s(data, True)

        # Energy detector to get signal burst
        ## use squelch to detect energy
        self.det  = analog.simple_squelch_cc(self.threshold, 0.01)
        ## convert to mag squared (float)
        self.c2m = blocks.complex_to_mag_squared()
        ## average to debounce
        self.avg = filter.single_pole_iir_filter_ff(0.01)
        ## rescale signal for conversion to short
        self.scale = blocks.multiply_const_ff(2**16)
        ## signal input uses shorts
        self.f2s = blocks.float_to_short()

        # Use file sink burst tagger to capture bursts
        self.fsnk = blocks.tagged_file_sink(gr.sizeof_gr_complex, self.samp_rate)


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
    parser = ArgumentParser()
    parser.add_argument("-a", "--address", default="addr=192.168.10.2",
                      help="select address of the device [default=%(default)r]")
    #parser.add_argument("-A", "--antenna", default=None,
    #                  help="select Rx Antenna (only on RFX-series boards)")
    parser.add_argument("-f", "--freq", type=eng_float, default=450e6,
                      help="set frequency to FREQ", metavar="FREQ")
    parser.add_argument("-g", "--gain", type=eng_float, default=0,
                      help="set gain in dB [default=%(default)r]")
    parser.add_argument("-R", "--samp-rate", type=eng_float, default=200000,
                      help="set USRP sample rate [default=%(default)r]")
    parser.add_argument("-t", "--threshold", type=float, default=-60,
                      help="Set the detection power threshold (dBm) [default=%(default)r")
    parser.add_argument("-T", "--trigger", action="store_true", default=False,
                      help="Use internal trigger instead of detector [default=%(default)r]")
    args = parser.parse_args()

    uhd_addr = args.address

    tb = uhd_burst_detector(uhd_addr, args)
    tb.run()
