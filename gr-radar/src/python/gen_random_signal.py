#!/usr/bin/env python
#
# Copyright 2005 Free Software Foundation, Inc.
# 
# This file is part of GNU Radio
# 
# GNU Radio is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2, or (at your option)
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

from gnuradio import gr, gru, blks, eng_notation
from gnuradio import audio
from gnuradio.eng_option import eng_option
from optparse import OptionParser
import sys
import math


class my_graph(gr.flow_graph):
    def __init__(self, output_filename, tx_gain, nsamples):
        gr.flow_graph.__init__(self)
        
        audio_rate = 31250
        baseband_rate = 250e3

        src = gr.noise_source_c(gr.GR_GAUSSIAN, 1.0)
        #src = gr.noise_source_f(gr.GR_GAUSSIAN, 0.5)
        #guts = blks.wfm_tx(self, audio_rate, baseband_rate)
        amp = gr.multiply_const_cc(tx_gain)
        head = gr.head(gr.sizeof_gr_complex, nsamples)
        sink = gr.file_sink(gr.sizeof_gr_complex, output_filename)
        self.connect(src, amp, head, sink)

def main ():
    usage = "usage: %prog [options] output_filename"
    parser = OptionParser (option_class=eng_option, usage=usage)
    parser.add_option("-N", "--nsamples", type="eng_float", default=250000,
                      help="specify number of output samples to generate[=250000]")
    parser.add_option("-g", "--tx-gain", type="eng_float", default=1.0,
                      help="specify transmitter gain[=1]")
    (options, args) = parser.parse_args ()

    if len (args) != 1:
        parser.print_help ()
        sys.exit (1)

    try:
        my_graph(args[0], options.tx_gain, int(options.nsamples)).run()
    except KeyboardInterrupt:
        pass
    

if __name__ == '__main__':
    main ()
