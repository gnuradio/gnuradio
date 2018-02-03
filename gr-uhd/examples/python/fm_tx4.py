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
Transmit N simultaneous narrow band FM signals.

They will be centered at the frequency specified on the command line,
and will spaced at 25kHz steps from there.

The program opens N files with names audio-N.dat where N is in [0,7].
These files should contain floating point audio samples in the range [-1,1]
sampled at 32kS/sec.  You can create files like this using
audio_to_file.py
"""

from gnuradio import gr, eng_notation
from gnuradio import uhd
from gnuradio import analog
from gnuradio import blocks
from gnuradio.eng_option import eng_option
from optparse import OptionParser
import math
import sys

from gnuradio.wxgui import stdgui2, fftsink2
import wx


########################################################
# instantiate one transmit chain for each call

class pipeline(gr.hier_block2):
    def __init__(self, filename, lo_freq, audio_rate, if_rate):

        gr.hier_block2.__init__(self, "pipeline",
                                gr.io_signature(0, 0, 0),
                                gr.io_signature(1, 1, gr.sizeof_gr_complex))

        try:
            src = blocks.file_source (gr.sizeof_float, filename, True)
        except RuntimeError:
            sys.stderr.write(("\nError: Could not open file '%s'\n\n" % \
                                  filename))
            sys.exit(1)

        print audio_rate, if_rate
        fmtx = analog.nbfm_tx(audio_rate, if_rate, max_dev=5e3,
	                      tau=75e-6, fh=0.925*if_rate/2.0)

        # Local oscillator
        lo = analog.sig_source_c(if_rate,            # sample rate
                                 analog.GR_SIN_WAVE, # waveform type
                                 lo_freq,            # frequency
                                 1.0,                # amplitude
                                 0)                  # DC Offset
        mixer = blocks.multiply_cc()

        self.connect(src, fmtx, (mixer, 0))
        self.connect(lo, (mixer, 1))
        self.connect(mixer, self)

class fm_tx_block(stdgui2.std_top_block):
    def __init__(self, frame, panel, vbox, argv):
        MAX_CHANNELS = 7
        stdgui2.std_top_block.__init__ (self, frame, panel, vbox, argv)

        parser = OptionParser (option_class=eng_option)
        parser.add_option("-a", "--args", type="string", default="",
                          help="UHD device address args [default=%default]")
        parser.add_option("", "--spec", type="string", default=None,
	                  help="Subdevice of UHD device where appropriate")
        parser.add_option("-A", "--antenna", type="string", default=None,
                          help="select Rx Antenna where appropriate")
        parser.add_option("-s", "--samp-rate", type="eng_float", default=400e3,
                          help="set sample rate (bandwidth) [default=%default]")
        parser.add_option("-f", "--freq", type="eng_float", default=None,
                          help="set frequency to FREQ", metavar="FREQ")
        parser.add_option("-g", "--gain", type="eng_float", default=None,
                          help="set gain in dB (default is midpoint)")
        parser.add_option("-n", "--nchannels", type="int", default=4,
                           help="number of Tx channels [1,4]")
        #parser.add_option("","--debug", action="store_true", default=False,
        #                  help="Launch Tx debugger")
        (options, args) = parser.parse_args ()

        if len(args) != 0:
            parser.print_help()
            sys.exit(1)

        if options.nchannels < 1 or options.nchannels > MAX_CHANNELS:
            sys.stderr.write ("fm_tx4: nchannels out of range.  Must be in [1,%d]\n" % MAX_CHANNELS)
            sys.exit(1)

        if options.freq is None:
            sys.stderr.write("fm_tx4: must specify frequency with -f FREQ\n")
            parser.print_help()
            sys.exit(1)

        # ----------------------------------------------------------------
        # Set up constants and parameters

        self.u = uhd.usrp_sink(device_addr=options.args, stream_args=uhd.stream_args('fc32'))

        # Set the subdevice spec
        if(options.spec):
            self.u.set_subdev_spec(options.spec, 0)

        # Set the antenna
        if(options.antenna):
            self.u.set_antenna(options.antenna, 0)

        self.usrp_rate = options.samp_rate
        self.u.set_samp_rate(self.usrp_rate)
        self.usrp_rate = self.u.get_samp_rate()

        self.sw_interp = 10
        self.audio_rate = self.usrp_rate / self.sw_interp    # 32 kS/s

        if options.gain is None:
            # if no gain was specified, use the mid-point in dB
            g = self.u.get_gain_range()
            options.gain = float(g.start()+g.stop())/2

        self.set_gain(options.gain)
        self.set_freq(options.freq)

        self.sum = blocks.add_cc ()

        # Instantiate N NBFM channels
        step = 25e3
        offset = (0 * step, 1 * step, -1 * step,
                  2 * step, -2 * step, 3 * step, -3 * step)

        for i in range (options.nchannels):
            t = pipeline("audio-%d.dat" % (i % 4), offset[i],
                         self.audio_rate, self.usrp_rate)
            self.connect(t, (self.sum, i))

        self.gain = blocks.multiply_const_cc (1.0 / options.nchannels)

        # connect it all
        self.connect (self.sum, self.gain)
        self.connect (self.gain, self.u)

        # plot an FFT to verify we are sending what we want
        if 1:
            post_mod = fftsink2.fft_sink_c(panel, title="Post Modulation",
                                           fft_size=512,
                                           sample_rate=self.usrp_rate,
                                           y_per_div=20,
                                           ref_level=40)
            self.connect (self.gain, post_mod)
            vbox.Add (post_mod.win, 1, wx.EXPAND)


        #if options.debug:
        #    self.debugger = tx_debug_gui.tx_debug_gui(self.subdev)
        #    self.debugger.Show(True)


    def set_freq(self, target_freq):
        """
        Set the center frequency we're interested in.

        Args:
            target_freq: frequency in Hz
        @rypte: bool

        Tuning is a two step process.  First we ask the front-end to
        tune as close to the desired frequency as it can.  Then we use
        the result of that operation and our target_frequency to
        determine the value for the digital up converter.  Finally, we feed
        any residual_freq to the s/w freq translator.
        """

        r = self.u.set_center_freq(target_freq, 0)
        if r:
            print "Frequency =", eng_notation.num_to_str(self.u.get_center_freq())
            return True

        return False

    def set_gain(self, gain):
        self.u.set_gain(gain, 0)


def main ():
    app = stdgui2.stdapp(fm_tx_block, "Multichannel FM Tx", nstatus=1)
    app.MainLoop ()

if __name__ == '__main__':
    main ()
