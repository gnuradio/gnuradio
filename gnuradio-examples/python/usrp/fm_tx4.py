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
from gnuradio import usrp
from gnuradio import audio
from gnuradio import blks2
from gnuradio.eng_option import eng_option
from optparse import OptionParser
from usrpm import usrp_dbid
import math
import sys

from gnuradio.wxgui import stdgui2, fftsink2
from gnuradio import tx_debug_gui
import wx


########################################################
# instantiate one transmit chain for each call

class pipeline(gr.hier_block2):
    def __init__(self, filename, lo_freq, audio_rate, if_rate):

        gr.hier_block2.__init__(self, "pipeline",
                                gr.io_signature(0, 0, 0),                    # Input signature
                                gr.io_signature(1, 1, gr.sizeof_gr_complex)) # Output signature

        src = gr.file_source (gr.sizeof_float, filename, True)
        fmtx = blks2.nbfm_tx (audio_rate, if_rate, max_dev=5e3, tau=75e-6)
        
        # Local oscillator
        lo = gr.sig_source_c (if_rate,        # sample rate
                              gr.GR_SIN_WAVE, # waveform type
                              lo_freq,        #frequency
                              1.0,            # amplitude
                              0)              # DC Offset
        mixer = gr.multiply_cc ()
    
        self.connect (src, fmtx, (mixer, 0))
        self.connect (lo, (mixer, 1))
        self.connect (mixer, self)

class fm_tx_block(stdgui2.std_top_block):
    def __init__(self, frame, panel, vbox, argv):
        MAX_CHANNELS = 7
        stdgui2.std_top_block.__init__ (self, frame, panel, vbox, argv)

        parser = OptionParser (option_class=eng_option)
        parser.add_option("-T", "--tx-subdev-spec", type="subdev", default=None,
                          help="select USRP Tx side A or B")
        parser.add_option("-f", "--freq", type="eng_float", default=None,
                           help="set Tx frequency to FREQ [required]", metavar="FREQ")
        parser.add_option("-n", "--nchannels", type="int", default=4,
                           help="number of Tx channels [1,4]")
        parser.add_option("","--debug", action="store_true", default=False,
                          help="Launch Tx debugger")
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

        self.u = usrp.sink_c ()       # the USRP sink (consumes samples)

        self.dac_rate = self.u.dac_rate()                    # 128 MS/s
        self.usrp_interp = 400
        self.u.set_interp_rate(self.usrp_interp)
        self.usrp_rate = self.dac_rate / self.usrp_interp    # 320 kS/s
        self.sw_interp = 10
        self.audio_rate = self.usrp_rate / self.sw_interp    # 32 kS/s

        # determine the daughterboard subdevice we're using
        if options.tx_subdev_spec is None:
            options.tx_subdev_spec = usrp.pick_tx_subdevice(self.u)

        m = usrp.determine_tx_mux_value(self.u, options.tx_subdev_spec)
        #print "mux = %#04x" % (m,)
        self.u.set_mux(m)
        self.subdev = usrp.selected_subdev(self.u, options.tx_subdev_spec)
        print "Using TX d'board %s" % (self.subdev.side_and_name(),)

        self.subdev.set_gain(self.subdev.gain_range()[1])    # set max Tx gain
        if not self.set_freq(options.freq):
            freq_range = self.subdev.freq_range()
            print "Failed to set frequency to %s.  Daughterboard supports %s to %s" % (
                eng_notation.num_to_str(options.freq),
                eng_notation.num_to_str(freq_range[0]),
                eng_notation.num_to_str(freq_range[1]))
            raise SystemExit
        self.subdev.set_enable(True)                         # enable transmitter

        sum = gr.add_cc ()

        # Instantiate N NBFM channels
        step = 25e3
        offset = (0 * step, 1 * step, -1 * step, 2 * step, -2 * step, 3 * step, -3 * step)
        for i in range (options.nchannels):
            t = pipeline("audio-%d.dat" % (i % 4), offset[i],
                         self.audio_rate, self.usrp_rate)
            self.connect(t, (sum, i))

        gain = gr.multiply_const_cc (4000.0 / options.nchannels)

        # connect it all
        self.connect (sum, gain)
        self.connect (gain, self.u)

        # plot an FFT to verify we are sending what we want
        if 1:
            post_mod = fftsink2.fft_sink_c(panel, title="Post Modulation",
                                           fft_size=512, sample_rate=self.usrp_rate,
                                           y_per_div=20, ref_level=40)
            self.connect (sum, post_mod)
            vbox.Add (post_mod.win, 1, wx.EXPAND)
            

        if options.debug:
            self.debugger = tx_debug_gui.tx_debug_gui(self.subdev)
            self.debugger.Show(True)


    def set_freq(self, target_freq):
        """
        Set the center frequency we're interested in.

        @param target_freq: frequency in Hz
        @rypte: bool

        Tuning is a two step process.  First we ask the front-end to
        tune as close to the desired frequency as it can.  Then we use
        the result of that operation and our target_frequency to
        determine the value for the digital up converter.  Finally, we feed
        any residual_freq to the s/w freq translater.
        """

        r = self.u.tune(self.subdev._which, self.subdev, target_freq)
        if r:
            print "r.baseband_freq =", eng_notation.num_to_str(r.baseband_freq)
            print "r.dxc_freq      =", eng_notation.num_to_str(r.dxc_freq)
            print "r.residual_freq =", eng_notation.num_to_str(r.residual_freq)
            print "r.inverted      =", r.inverted
            
            # Could use residual_freq in s/w freq translator
            return True

        return False

def main ():
    app = stdgui2.stdapp(fm_tx_block, "Multichannel FM Tx", nstatus=1)
    app.MainLoop ()

if __name__ == '__main__':
    main ()
