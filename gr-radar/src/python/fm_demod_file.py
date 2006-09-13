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

"""
Demod FM signal in data collected for radar.
Input samples are complex baseband, 250kS/sec.
"""

from gnuradio import gr, gru, blks, eng_notation
from gnuradio import audio
from gnuradio.eng_option import eng_option
from optparse import OptionParser
import sys
import math


class my_graph(gr.flow_graph):
    def __init__(self, input_filename, repeat):
        gr.flow_graph.__init__(self)
        
        baseband_rate = 250e3
        audio_decim = 8
        audio_rate = int(baseband_rate // audio_decim) # output is at 31250 S/s

        src = gr.file_source(gr.sizeof_gr_complex, input_filename, repeat)
        guts = blks.wfm_rcv(self, baseband_rate, audio_decim)  
        sink = audio.sink(audio_rate, "plughw:0,0")
        self.connect(src, guts, sink)

def main ():
    usage = "usage: %prog [options] filename"
    parser = OptionParser (option_class=eng_option, usage=usage)
    parser.add_option ("-r", "--repeat", action="store_true", default=False)
    (options, args) = parser.parse_args ()

    if len (args) != 1:
        parser.print_help ()
        sys.exit (1)

    try:
        my_graph(args[0], options.repeat).run()
    except KeyboardInterrupt:
        pass
    

if __name__ == '__main__':
    main ()
