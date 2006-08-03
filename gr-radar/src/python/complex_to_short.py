#!/usr/bin/env python

from gnuradio import gr, gru, eng_notation, optfir
from gnuradio.eng_option import eng_option
from optparse import OptionParser
import os.path
import re

class my_graph(gr.flow_graph):
    def __init__(self):
        gr.flow_graph.__init__(self)

        usage="%prog: [options] input_file output_file"
        parser = OptionParser (option_class=eng_option, usage=usage)
        (options, args) = parser.parse_args()
        if len(args) != 2:
            parser.print_help()
            raise SystemExit, 1

        input_filename = args[0]
        output_filename = args[1]

        inf = gr.file_source(gr.sizeof_gr_complex, input_filename)
        c2s = gr.complex_to_interleaved_short()
        outf = gr.file_sink(gr.sizeof_short, output_filename)
        self.connect(inf, c2s, outf)


if __name__ == '__main__':
    try:
        my_graph().run()
    except KeyboardInterrupt:
        pass
