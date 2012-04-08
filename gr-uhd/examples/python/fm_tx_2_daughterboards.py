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

"""
Transmit 2 signals, one out each daughterboard.

Outputs SSB (USB) signals on side A and side B at frequencies
specified on command line.

Side A is 600 Hz tone.
Side B is 350 + 440 Hz tones.
"""

from gnuradio import gr, uhd, blks2
from gnuradio.eng_notation import num_to_str, str_to_num
from gnuradio.eng_option import eng_option
from optparse import OptionParser
import math
import sys


class example_signal_0(gr.hier_block2):
    """
    Sinusoid at 600 Hz.
    """
    def __init__(self, sample_rate):
        gr.hier_block2.__init__(self, "example_signal_0",
                                gr.io_signature(0, 0, 0),                    # Input signature
                                gr.io_signature(1, 1, gr.sizeof_gr_complex)) # Output signature

        src = gr.sig_source_c (sample_rate,    # sample rate
                               gr.GR_SIN_WAVE, # waveform type
                               600,            # frequency
                               1.0,            # amplitude
                               0)              # DC Offset
    
        self.connect(src, self)


class example_signal_1(gr.hier_block2):
    """
    North American dial tone (350 + 440 Hz).
    """
    def __init__(self, sample_rate):
        gr.hier_block2.__init__(self, "example_signal_1",
                                gr.io_signature(0, 0, 0),                    # Input signature
                                gr.io_signature(1, 1, gr.sizeof_gr_complex)) # Output signature

        src0 = gr.sig_source_c (sample_rate,    # sample rate
                                gr.GR_SIN_WAVE, # waveform type
                                350,            # frequency
                                1.0,            # amplitude
                                0)              # DC Offset

        src1 = gr.sig_source_c (sample_rate,    # sample rate
                                gr.GR_SIN_WAVE, # waveform type
                                440,            # frequency
                                1.0,            # amplitude
                                0)              # DC Offset
        sum = gr.add_cc()
        self.connect(src0, (sum, 0))
        self.connect(src1, (sum, 1))
        self.connect(sum, self)

class my_top_block(gr.top_block):

    def __init__(self):
        gr.top_block.__init__(self)

        usage="%prog: [options] tx-freq0 tx-freq1"
        parser = OptionParser (option_class=eng_option, usage=usage)
        parser.add_option("-a", "--args", type="string", default="",
                          help="UHD device address args [default=%default]")
        parser.add_option("", "--spec", type="string", default=None,
	                  help="Subdevice of UHD device where appropriate")
        parser.add_option("-A", "--antenna", type="string", default=None,
                          help="select Rx Antenna where appropriate")
        parser.add_option("-s", "--samp-rate", type="eng_float", default=320e3,
                          help="set sample rate [default=%default]")
        parser.add_option("-g", "--gain", type="eng_float", default=None,
                          help="set gain in dB (default is midpoint)")
        (options, args) = parser.parse_args ()

        if len(args) != 2:
            parser.print_help()
            raise SystemExit
        else:
            freq0 = str_to_num(args[0])
            freq1 = str_to_num(args[1])

        # ----------------------------------------------------------------
        # Set up USRP to transmit on both daughterboards

        d = uhd.find_devices(uhd.device_addr(options.args))
        uhd_type = d[0].get('type')

        stream_args = uhd.stream_args('fc32', channels=range(2))
        self.u = uhd.usrp_sink(device_addr=options.args, stream_args=stream_args)

        # Set up USRP system based on type
        if(uhd_type == "usrp"):
            self.u.set_subdev_spec("A:0 B:0")
            tr0 = uhd.tune_request(freq0)
            tr1 = uhd.tune_request(freq1)

        else:
            if abs(freq0 - freq1) > 5.5e6:
                sys.stderr.write("\nError: When not using two separate d'boards, frequencies must bewithin 5.5MHz of each other.\n")
                raise SystemExit

            self.u.set_subdev_spec("A:0 A:0")

            mid_freq = (freq0 + freq1)/2.0
            tr0 = uhd.tune_request(freq0, rf_freq=mid_freq,
                                   rf_freq_policy=uhd.tune_request.POLICY_MANUAL)

            tr1 = uhd.tune_request(freq1, rf_freq=mid_freq,
                                   rf_freq_policy=uhd.tune_request.POLICY_MANUAL)

        # Use the tune requests to tune each channel
        self.set_freq(tr0, 0)
        self.set_freq(tr1, 1)

        self.usrp_rate  = options.samp_rate

        self.u.set_samp_rate(self.usrp_rate)
        dev_rate = self.u.get_samp_rate()

        # ----------------------------------------------------------------
        # build two signal sources, interleave them, amplify and
        # connect them to usrp

        sig0 = example_signal_0(self.usrp_rate)
        sig1 = example_signal_1(self.usrp_rate)

        intl = gr.interleave(gr.sizeof_gr_complex)
        self.connect(sig0, (intl, 0))
        self.connect(sig1, (intl, 1))

        # Correct for any difference in requested and actual rates
        rrate = self.usrp_rate / dev_rate
        resamp = blks2.pfb_arb_resampler_ccf(rrate)

        # and wire them up
        self.connect(intl, resamp, self.u)

        if options.gain is None:
            # if no gain was specified, use the mid-point in dB
            g = self.u.get_gain_range()
            options.gain = float(g.start()+g.stop())/2.0

        self.set_gain(options.gain, 0)
        self.set_gain(options.gain, 1)

        # Set the subdevice spec
        if(options.spec):
            self.u.set_subdev_spec(options.spec, 0)

        # Set the antenna
        if(options.antenna):
            self.u.set_antenna(options.antenna, 0)
            self.u.set_antenna(options.antenna, 1)
       
    def set_freq(self, target_freq, chan):
        """
        Set the center frequency we're interested in.

        @param side: 0 = side A, 1 = side B
        @param target_freq: frequency in Hz
        @rtype: bool
        """

        print "Tuning channel %s to %sHz" % \
            (chan, num_to_str(target_freq))

        r = self.u.set_center_freq(target_freq, chan)

        if r:
            return True

        else:
            print "  Set Frequency Failed!"
            
        return False

    def set_gain(self, gain, chan):
        self.u.set_gain(gain, chan)

if __name__ == '__main__':
    try:
        my_top_block().run()
    except KeyboardInterrupt:
        pass
