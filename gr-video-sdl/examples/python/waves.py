#!/usr/bin/env python3
# -*- coding: utf-8 -*-

#
# SPDX-License-Identifier: GPL-3.0
#
# GNU Radio Python Flow Graph
# Title: SDL Demo
# Author: Marcus Müller, Oleg Nikitin
# GNU Radio version: g935569cc9

from gnuradio import analog
from gnuradio import blocks
from gnuradio import gr
from gnuradio.filter import firdes
from gnuradio.fft import window
import sys
import signal
from argparse import ArgumentParser
from gnuradio.eng_arg import eng_float, intx
from gnuradio import eng_notation
from gnuradio import video_sdl
import threading


class waves(gr.top_block):
    def __init__(self, num_rows=512, periods_per_row=(1 - 1 / 255), update_rate=25, vector_length=512):
        gr.top_block.__init__(self, "SDL Demo", catch_exceptions=True)
        self.flowgraph_started = threading.Event()

        ##################################################
        # Parameters
        ##################################################
        self.num_rows = num_rows
        self.periods_per_row = periods_per_row
        self.update_rate = update_rate
        self.vector_length = vector_length

        ##################################################
        # Blocks
        ##################################################

        self.video_sdl_sink_0 = video_sdl.sink_s(0, vector_length, num_rows, vector_length, num_rows)
        self.blocks_throttle2_0 = blocks.throttle(
            gr.sizeof_float * 1,
            update_rate * vector_length * num_rows,
            True,
            0
            if "time" == "auto"
            else max(
                int(float((0.25 / update_rate)) * (update_rate * vector_length * num_rows))
                if "time" == "time"
                else int((0.25 / update_rate)),
                1,
            ),
        )
        self.blocks_float_to_short_0 = blocks.float_to_short(1, (2**6))
        self.analog_sig_source_x_0 = analog.sig_source_f(
            (update_rate * vector_length * num_rows),
            analog.GR_COS_WAVE,
            (update_rate * periods_per_row * num_rows),
            1,
            1,
            0,
        )

        ##################################################
        # Connections
        ##################################################
        self.connect((self.analog_sig_source_x_0, 0), (self.blocks_throttle2_0, 0))
        self.connect((self.blocks_float_to_short_0, 0), (self.video_sdl_sink_0, 0))
        self.connect((self.blocks_throttle2_0, 0), (self.blocks_float_to_short_0, 0))

    def get_num_rows(self):
        return self.num_rows

    def set_num_rows(self, num_rows):
        self.num_rows = num_rows
        self.analog_sig_source_x_0.set_sampling_freq((self.update_rate * self.vector_length * self.num_rows))
        self.analog_sig_source_x_0.set_frequency((self.update_rate * self.periods_per_row * self.num_rows))
        self.blocks_throttle2_0.set_sample_rate((self.update_rate * self.vector_length * self.num_rows))

    def get_periods_per_row(self):
        return self.periods_per_row

    def set_periods_per_row(self, periods_per_row):
        self.periods_per_row = periods_per_row
        self.analog_sig_source_x_0.set_frequency((self.update_rate * self.periods_per_row * self.num_rows))

    def get_update_rate(self):
        return self.update_rate

    def set_update_rate(self, update_rate):
        self.update_rate = update_rate
        self.analog_sig_source_x_0.set_sampling_freq((self.update_rate * self.vector_length * self.num_rows))
        self.analog_sig_source_x_0.set_frequency((self.update_rate * self.periods_per_row * self.num_rows))
        self.blocks_throttle2_0.set_sample_rate((self.update_rate * self.vector_length * self.num_rows))

    def get_vector_length(self):
        return self.vector_length

    def set_vector_length(self, vector_length):
        self.vector_length = vector_length
        self.analog_sig_source_x_0.set_sampling_freq((self.update_rate * self.vector_length * self.num_rows))
        self.blocks_throttle2_0.set_sample_rate((self.update_rate * self.vector_length * self.num_rows))


def argument_parser():
    parser = ArgumentParser()
    parser.add_argument(
        "-n", "--num-rows", dest="num_rows", type=intx, default=512,
        help="Set Number of Rows [default=%(default)r]")
    parser.add_argument(
        "-p", "--periods-per-row", dest="periods_per_row",
        type=eng_float, default=eng_notation.num_to_str(float((1 - 1 / 255))),
        help="Set Periods per Row [default=%(default)r]")
    parser.add_argument(
        "-r", "--update-rate", dest="update_rate", type=intx, default=25,
        help="Set Update Rate [default=%(default)r]")
    parser.add_argument(
        "-l", "--vector-length", dest="vector_length", type=intx, default=512,
        help="Set Vector Length [default=%(default)r]")
    return parser


def main(top_block_cls=waves, options=None):
    if options is None:
        options = argument_parser().parse_args()
    tb = top_block_cls(
        num_rows=options.num_rows,
        periods_per_row=options.periods_per_row,
        update_rate=options.update_rate,
        vector_length=options.vector_length,
    )

    def sig_handler(sig=None, frame=None):
        tb.stop()
        tb.wait()

        sys.exit(0)

    signal.signal(signal.SIGINT, sig_handler)
    signal.signal(signal.SIGTERM, sig_handler)

    tb.start()
    tb.flowgraph_started.set()

    try:
        input('Press Enter to quit: ')
    except EOFError:
        pass
    tb.stop()
    tb.wait()


if __name__ == '__main__':
    main()
