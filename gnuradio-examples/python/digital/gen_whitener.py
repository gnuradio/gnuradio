#!/usr/bin/env python

from gnuradio import gr, gru
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

        src = gr.lfsr_32k_source_s()
        head = gr.head(gr.sizeof_short, 2048)
        self.dst = gr.vector_sink_s()
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
    
    
