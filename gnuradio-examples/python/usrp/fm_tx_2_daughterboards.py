#!/usr/bin/env python

"""
Transmit 2 signals, one out each daughterboard.

Outputs SSB (USB) signals on side A and side B at frequencies
specified on command line.

Side A is 600 Hz tone.
Side B is 350 + 440 Hz tones.
"""

from gnuradio import gr
from gnuradio.eng_notation import num_to_str, str_to_num
from gnuradio import usrp
from gnuradio import audio
from gnuradio import blks
from gnuradio.eng_option import eng_option
from optparse import OptionParser
import usrp_dbid
import math
import sys


class example_signal_0(gr.hier_block):
    """
    Sinusoid at 600 Hz.
    """
    def __init__(self, fg, sample_rate):

        src = gr.sig_source_c (sample_rate,    # sample rate
                               gr.GR_SIN_WAVE, # waveform type
                               600,            # frequency
                               1.0,            # amplitude
                               0)              # DC Offset
    
        gr.hier_block.__init__(self, fg, None, src)


class example_signal_1(gr.hier_block):
    """
    North American dial tone (350 + 440 Hz).
    """
    def __init__(self, fg, sample_rate):

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
        fg.connect(src0, (sum, 0))
        fg.connect(src1, (sum, 1))
        
        gr.hier_block.__init__(self, fg, None, sum)
    


class my_graph(gr.flow_graph):

    def __init__(self):
        gr.flow_graph.__init__ (self)

        usage="%prog: [options] side-A-tx-freq side-B-tx-freq"
        parser = OptionParser (option_class=eng_option, usage=usage)
        (options, args) = parser.parse_args ()

        if len(args) != 2:
            parser.print_help()
            raise SystemExit
        else:
            freq0 = str_to_num(args[0])
            freq1 = str_to_num(args[1])

        # ----------------------------------------------------------------
        # Set up USRP to transmit on both daughterboards

        self.u = usrp.sink_c(nchan=2)          # say we want two channels

        self.dac_rate = self.u.dac_rate()                    # 128 MS/s
        self.usrp_interp = 400
        self.u.set_interp_rate(self.usrp_interp)
        self.usrp_rate = self.dac_rate / self.usrp_interp    # 320 kS/s

        # we're using both daughterboard slots, thus subdev is a 2-tuple
        self.subdev = (self.u.db[0][0], self.u.db[1][0])
        print "Using TX d'board %s" % (self.subdev[0].side_and_name(),)
        print "Using TX d'board %s" % (self.subdev[1].side_and_name(),)
        
        # set up the Tx mux so that
        #  channel 0 goes to Slot A I&Q and channel 1 to Slot B I&Q
        self.u.set_mux(0xba98)

        self.subdev[0].set_gain(self.subdev[0].gain_range()[1])    # set max Tx gain
        self.subdev[1].set_gain(self.subdev[1].gain_range()[1])    # set max Tx gain

        self.set_freq(0, freq0)
        self.set_freq(1, freq1)
        self.subdev[0].set_enable(True)             # enable transmitter
        self.subdev[1].set_enable(True)             # enable transmitter

        # ----------------------------------------------------------------
        # build two signal sources, interleave them, amplify and connect them to usrp

        sig0 = example_signal_0(self, self.usrp_rate)
        sig1 = example_signal_1(self, self.usrp_rate)

        intl = gr.interleave(gr.sizeof_gr_complex)
        self.connect(sig0, (intl, 0))
        self.connect(sig1, (intl, 1))

        # apply some gain
        if_gain = 10000
        ifamp = gr.multiply_const_cc(if_gain)
        
        # and wire them up
        self.connect(intl, ifamp, self.u)
        

    def set_freq(self, side, target_freq):
        """
        Set the center frequency we're interested in.

        @param side: 0 = side A, 1 = side B
        @param target_freq: frequency in Hz
        @rtype: bool

        Tuning is a two step process.  First we ask the front-end to
        tune as close to the desired frequency as it can.  Then we use
        the result of that operation and our target_frequency to
        determine the value for the digital up converter.
        """

        print "Tuning side %s to %sHz" % (("A", "B")[side], num_to_str(target_freq))
        r = self.u.tune(self.subdev[side]._which, self.subdev[side], target_freq)
        if r:
            print "  r.baseband_freq =", num_to_str(r.baseband_freq)
            print "  r.dxc_freq      =", num_to_str(r.dxc_freq)
            print "  r.residual_freq =", num_to_str(r.residual_freq)
            print "  r.inverted      =", r.inverted
            print "  OK"
            return True

        else:
            print "  Failed!"
            
        return False


if __name__ == '__main__':
    try:
        my_graph().run()
    except KeyboardInterrupt:
        pass
