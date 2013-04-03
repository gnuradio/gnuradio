#!/usr/bin/env python
#
# Copyright 2004,2005,2007,2008,2012,2013 Free Software Foundation, Inc.
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

# GNU Radio example program to record a dial tone to a WAV file

from gnuradio import gr
from gnuradio import blocks
from gnuradio.eng_option import eng_option
from optparse import OptionParser

try:
    from gnuradio import analog
except ImportError:
    sys.stderr.write("Error: Program requires gr-analog.\n")
    sys.exit(1)

class my_top_block(gr.top_block):

    def __init__(self):
        gr.top_block.__init__(self)

	usage = "%prog: [options] filename"
        parser = OptionParser(option_class=eng_option, usage=usage)
        parser.add_option("-r", "--sample-rate", type="eng_float", default=48000,
                          help="set sample rate to RATE (48000)")
	parser.add_option("-N", "--samples", type="eng_float", default=None,
			  help="number of samples to record")
        (options, args) = parser.parse_args ()
        if len(args) != 1 or options.samples is None:
            parser.print_help()
            raise SystemExit, 1

        sample_rate = int(options.sample_rate)
        ampl = 0.1

        src0 = analog.sig_source_f(sample_rate, analog.GR_SIN_WAVE, 350, ampl)
        src1 = analog.sig_source_f(sample_rate, analog.GR_SIN_WAVE, 440, ampl)
	head0 = blocks.head(gr.sizeof_float, int(options.samples))
	head1 = blocks.head(gr.sizeof_float, int(options.samples))
	dst = blocks.wavfile_sink(args[0], 2, int(options.sample_rate), 16)

        self.connect(src0, head0, (dst, 0))
        self.connect(src1, head1, (dst, 1))

if __name__ == '__main__':
    try:
        my_top_block().run()
    except KeyboardInterrupt:
        pass
