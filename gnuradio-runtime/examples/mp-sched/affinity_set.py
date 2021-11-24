#!/usr/bin/env python
##################################################
# Gnuradio Python Flow Graph
# Title: Affinity Set Test
##################################################

from gnuradio import eng_notation
from gnuradio import gr
from gnuradio import blocks
from gnuradio import filter
from gnuradio.eng_arg import eng_arg
from gnuradio.filter import firdes
from argparse import ArgumentParser
import sys


class affinity_set(gr.top_block):

    def __init__(self):
        gr.top_block.__init__(self, "Affinity Set Test")

        ##################################################
        # Variables
        ##################################################
        self.samp_rate = samp_rate = 32000

        ##################################################
        # Blocks
        ##################################################
        vec_len = 1
        self.blocks_throttle_0 = blocks.throttle(
            gr.sizeof_gr_complex * vec_len, samp_rate)
        self.blocks_null_source_0 = blocks.null_source(
            gr.sizeof_gr_complex * vec_len)
        self.blocks_null_sink_0 = blocks.null_sink(
            gr.sizeof_gr_complex * vec_len)
        self.filter_filt_0 = filter.fir_filter_ccc(1, 40000 * [0.2 + 0.3j, ])
        self.filter_filt_1 = filter.fir_filter_ccc(1, 40000 * [0.2 + 0.3j, ])

        self.filter_filt_0.set_processor_affinity([0, ])
        self.filter_filt_1.set_processor_affinity([0, 1])

        ##################################################
        # Connections
        ##################################################
        self.connect((self.blocks_null_source_0, 0),
                     (self.blocks_throttle_0, 0))
        self.connect((self.blocks_throttle_0, 0), (self.filter_filt_0, 0))
        self.connect((self.filter_filt_0, 0), (self.filter_filt_1, 0))
        self.connect((self.filter_filt_1, 0), (self.blocks_null_sink_0, 0))

    def get_samp_rate(self):
        return self.samp_rate

    def set_samp_rate(self, samp_rate):
        self.samp_rate = samp_rate


if __name__ == '__main__':
    parser = ArgumentParser()
    args = parser.parse_args()
    tb = affinity_set()
    tb.start()

    while(1):
        ret = input('Enter a new Core # or Press Enter to quit: ')
        if(len(ret) == 0):
            tb.stop()
            sys.exit(0)
        elif(ret.lower() == "none"):
            tb.filter_filt_0.unset_processor_affinity()
        else:
            try:
                n = int(ret)
            except ValueError:
                print("Invalid number")
            else:
                tb.filter_filt_0.set_processor_affinity([n, ])
