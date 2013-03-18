#!/usr/bin/env python
#
# Copyright 2011,2013 Free Software Foundation, Inc.
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

from gnuradio import gr, gru
from gnuradio import blocks
from gnuradio.eng_option import eng_option
from optparse import OptionParser
import sys

class my_graph(gr.top_block):

    def __init__(self):
        gr.top_block.__init__(self)

        parser = OptionParser(option_class=eng_option)
        (options, args) = parser.parse_args ()
        if len(args) != 0:
            parser.print_help()
            raise SystemExit, 1

        src = blocks.lfsr_32k_source_s()
        head = blocks.head(gr.sizeof_short, 2048)
        self.dst = blocks.vector_sink_s()
        self.connect(src, head, self.dst)

if __name__ == '__main__':
    try:
        tb = my_graph()
        tb.run()
        f = sys.stdout
        i = 0
        for s in tb.dst.data():
            f.write("%3d, " % (s & 0xff,))
            f.write("%3d, " % ((s >> 8) & 0xff,))
            i = i+2
            if i % 16 == 0:
                f.write('\n')
                    
    except KeyboardInterrupt:
        pass
    
    
