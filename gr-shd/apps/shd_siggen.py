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

"""
Read samples from a SHD device and write to file formatted as binary
outputs single precision complex float values or complex short values 
(interleaved 16 bit signed short integers).
"""

from gnuradio import gr, eng_notation
from gnuradio import shd
from gnuradio.eng_option import eng_option
from optparse import OptionParser
import sys

n2s = eng_notation.num_to_str

class shd_siggen(gr.top_block):

    def __init__(self, options):
        gr.top_block.__init__(self)

        self._src = gr.sig_source_c(options.samp_rate, gr.GR_SIN_WAVE,
                                    200, 1)

        self._snk = shd.smini_sink(device_addr=options.address,
                                   io_type=shd.io_type.COMPLEX_FLOAT32,
                                   num_channels=1)

        # Set receiver sample rate
        self._snk.set_samp_rate(options.samp_rate)

        # Set receive daughterboard gain
        if options.gain is None:
            g = self._snk.get_gain_range()
            options.gain = float(g.start()+g.stop())/2
	    print "Using mid-point gain of", \
                options.gain, "(", g.start(), "-", g.stop(), ")"
        self._snk.set_gain(options.gain)

        # Set the antenna
        if(options.antenna):
            self._snk.set_antenna(options.antenna, 0)

        # Set frequency (tune request takes lo_offset)
        if(options.lo_offset is not None):
            treq = shd.tune_request(options.freq, options.lo_offset)
        else:
            treq = shd.tune_request(options.freq)
        tr = self._snk.set_center_freq(treq)
        if tr == None:
            sys.stderr.write('Failed to set center frequency\n')
            raise SystemExit, 1

        # Create head block if needed and wire it up
        self.connect(self._src, self._snk)
        input_rate = self._snk.get_samp_rate()
        
        if options.verbose:
            print "Address:", options.address
            print "Rx gain:", options.gain
            print "Rx baseband frequency:", n2s(tr.actual_rf_freq)
            print "Rx DDC frequency:", n2s(tr.actual_dsp_freq)
            print "Rx Sample Rate:", n2s(input_rate)
        
def get_options():
    usage="%prog: [options]"
    parser = OptionParser(option_class=eng_option, usage=usage)
    parser.add_option("-a", "--address", type="string", default="type=xmini",
                      help="Address of SHD device, [default=%default]")
    parser.add_option("-A", "--antenna", type="string", default=None,
                      help="select Rx Antenna where appropriate")
    parser.add_option("", "--samp-rate", type="eng_float", default=1e6,
                      help="set sample rate (bandwidth) [default=%default]")
    parser.add_option("-f", "--freq", type="eng_float", default=None,
                      help="set frequency to FREQ", metavar="FREQ")
    parser.add_option("-g", "--gain", type="eng_float", default=None,
                      help="set gain in dB (default is midpoint)")
    parser.add_option("-v", "--verbose", action="store_true", default=False,
                      help="verbose output")
    parser.add_option("", "--lo-offset", type="eng_float", default=None,
                      help="set daughterboard LO offset to OFFSET [default=hw default]")

    (options, args) = parser.parse_args ()

    if options.freq is None:
        parser.print_help()
        sys.stderr.write('You must specify the frequency with -f FREQ\n');
        raise SystemExit, 1
    
    return (options)


if __name__ == '__main__':
    options = get_options()
    tb = shd_siggen(options)
  
    try:
        tb.run()
    except KeyboardInterrupt:
        pass
