#!/usr/bin/env python
#
# Copyright 2007 Free Software Foundation, Inc.
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.
#

from gnuradio import gr, eng_notation
from gnuradio.eng_option import eng_option
from optparse import OptionParser
import sys

master_clock = 100e6

class my_top_block(gr.top_block):

    def __init__(self):
        gr.top_block.__init__(self)

        parser = OptionParser(option_class=eng_option)
        parser.add_option("-f", "--freq", type="eng_float", default=1e6,
                          help="set waveform frequency to FREQ [default=%default]")
        parser.add_option ("-a", "--amplitude", type="eng_float", default=16e3,
                           help="set waveform amplitude to AMPLITUDE [default=%default]", metavar="AMPL")

        parser.add_option("-i", "--interp", type="int", default=32,
                          help="assume fgpa interpolation rate is INTERP [default=%default]")

        (options, args) = parser.parse_args ()
        if len(args) != 0:
            parser.print_help()
            raise SystemExit, 1

        
        src0 = gr.sig_source_c(master_clock/options.interp,
                               gr.GR_SIN_WAVE,
                               options.freq,
                               options.amplitude)

        
        c2s = gr.complex_to_interleaved_short()

        stdout_sink = gr.file_descriptor_sink(gr.sizeof_short, 1)

        self.connect(src0, c2s, stdout_sink)

        
if __name__ == '__main__':
    try:
        my_top_block().run()
    except KeyboardInterrupt:
        pass
