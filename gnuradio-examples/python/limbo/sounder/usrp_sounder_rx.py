#!/usr/bin/env python
#
# Copyright 2007 Free Software Foundation, Inc.
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

from gnuradio import gr, eng_notation
from gnuradio.eng_option import eng_option
from optparse import OptionParser
from usrp_source import usrp_source_c
from sounder_rx import sounder_rx

n2s = eng_notation.num_to_str

class usrp_sounder_rx(gr.top_block):
    def __init__(self, options):
        gr.top_block.__init__(self, "usrp_sounder_rx")
        self._options = options
        self._u = usrp_source_c(0,
                                self._options.rx_subdev_spec,
                                self._options.gain,
                                self._options.chip_rate,
                                self._options.freq,
                                self._options.cal,
                                self._options.verbose)
        self._options.chip_rate = self._u._if_rate
        self._length = 2**self._options.degree-1
        self._receiver = sounder_rx(self._options.chip_rate,
                                    self._options.degree,
                                    self._options.verbose)

        samples = 100 * self._length**2

        head = gr.head(gr.sizeof_gr_complex, samples)
        c2m = gr.complex_to_mag()
        s2v = gr.stream_to_vector(gr.sizeof_float, self._length)
        lpf = gr.single_pole_iir_filter_ff(self._options.alpha, self._length)
        v2s = gr.vector_to_stream(gr.sizeof_float, self._length)
        sink = gr.file_sink(gr.sizeof_float, "impulse.dat")

	self.connect(self._u, head, self._receiver, c2m, s2v, lpf, v2s, sink)

	if self._options.verbose:
            print "Chip rate is", n2s(self._options.chip_rate), "chips/sec"
            print "Resolution is", n2s(1.0/self._options.chip_rate), "sec"
            print "Using PN code of degree", self._options.degree

def main():
    parser = OptionParser(option_class=eng_option)
    parser.add_option("-R", "--rx-subdev-spec", type="subdev", default=None,
                      help="select USRP Rx side A or B (default=first found)")
    parser.add_option("-f", "--freq", type="eng_float", default=0.0,
                      help="set center frequency (default=%default)")
    parser.add_option("-c", "--cal", type="eng_float", default=0.0,
                      help="set frequency calibration offset (default=%default)")
    parser.add_option("-v", "--verbose", action="store_true", default=False,
                      help="print extra debugging info")
    parser.add_option("-d", "--degree", type="int", default=10,
                      help="set PN code degree (length=2**degree-1, default=%default)")
    parser.add_option("-r", "--chip-rate", type="eng_float", default=8e6,
                      help="set sounder chip rate (default=%default)")
    parser.add_option("-g", "--gain", type="eng_float", default=None,
                      help="set receiver gain (default=%default)")
    parser.add_option("", "--alpha", type="eng_float", default=1.0,
                      help="set smoothing constant (default=%default)")
    (options, args) = parser.parse_args()
    if len(args) != 0:
        parser.print_help()
        sys.exit(1)

    top_block = usrp_sounder_rx(options)

    try:
        top_block.run()
    except KeyboardInterrupt:
        pass

if __name__ == '__main__':
    main ()
