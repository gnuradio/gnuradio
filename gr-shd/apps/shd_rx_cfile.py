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

class rx_cfile_block(gr.top_block):

    def __init__(self, options, filename):
        gr.top_block.__init__(self)

        # Create a SHD device source
        if options.output_shorts:
            self._src = shd.smini_source(device_addr=options.address,
                                       io_type=shd.io_type.COMPLEX_INT16,
                                       num_channels=1)
            self._sink = gr.file_sink(gr.sizeof_short*2, filename)
        else:
            self._src = shd.smini_source(device_addr=options.address,
                                       io_type=shd.io_type.COMPLEX_FLOAT32,
                                       num_channels=1)
            self._sink = gr.file_sink(gr.sizeof_gr_complex, filename)
            
        # Set receiver sample rate
        self._src.set_samp_rate(options.samp_rate)

        # Set receive daughterboard gain
        if options.gain is None:
            g = self._src.get_gain_range()
            options.gain = float(g.start()+g.stop())/2
	    print "Using mid-point gain of", \
                options.gain, "(", g.start(), "-", g.stop(), ")"
        self._src.set_gain(options.gain)

        # Set the antenna
        if(options.antenna):
            self._src.set_antenna(options.antenna, 0)

        # Set frequency (tune request takes lo_offset)
        if(options.lo_offset is not None):
            treq = shd.tune_request(options.freq, options.lo_offset)
        else:
            treq = shd.tune_request(options.freq)
        tr = self._src.set_center_freq(treq)
        if tr == None:
            sys.stderr.write('Failed to set center frequency\n')
            raise SystemExit, 1

        # Create head block if needed and wire it up
        if options.nsamples is None:
            self.connect(self._src, self._sink)
        else:
            if options.output_shorts:
                self._head = gr.head(gr.sizeof_short*2,
                                     int(options.nsamples))
            else:
                self._head = gr.head(gr.sizeof_gr_complex,
                                     int(options.nsamples))

            self.connect(self._src, self._head, self._sink)

        input_rate = self._src.get_samp_rate()
        
        if options.verbose:
            print "Address:", options.address
            print "Rx gain:", options.gain
            print "Rx baseband frequency:", n2s(tr.actual_rf_freq)
            print "Rx DDC frequency:", n2s(tr.actual_dsp_freq)
            print "Rx Sample Rate:", n2s(input_rate)
            if options.nsamples is None:
                print "Receiving samples until Ctrl-C"
            else:
                print "Receving", n2s(options.nsamples), "samples"
            if options.output_shorts:
                print "Writing 16-bit complex shorts"
            else:
                print "Writing 32-bit complex floats"
            print "Output filename:", filename
        
def get_options():
    usage="%prog: [options] output_filename"
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
    parser.add_option( "-s","--output-shorts", action="store_true", default=False,
                      help="output interleaved shorts instead of complex floats")
    parser.add_option("-N", "--nsamples", type="eng_float", default=None,
                      help="number of samples to collect [default=+inf]")
    parser.add_option("-v", "--verbose", action="store_true", default=False,
                      help="verbose output")
    parser.add_option("", "--lo-offset", type="eng_float", default=None,
                      help="set daughterboard LO offset to OFFSET [default=hw default]")

    (options, args) = parser.parse_args ()
    if len(args) != 1:
        parser.print_help()
        raise SystemExit, 1
    
    if options.freq is None:
        parser.print_help()
        sys.stderr.write('You must specify the frequency with -f FREQ\n');
        raise SystemExit, 1
    
    return (options, args[0])


if __name__ == '__main__':
    (options, filename) = get_options()
    tb = rx_cfile_block(options, filename)
    
    try:
        tb.run()
    except KeyboardInterrupt:
        pass
