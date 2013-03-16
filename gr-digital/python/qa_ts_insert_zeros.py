#!/usr/bin/env python
# Copyright 2012 Free Software Foundation, Inc.
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

import time
import itertools

from gnuradio import gr, gr_unittest
try: import pmt
except: from gruel import pmt
import blocks_swig as blocks
import digital_swig as digital
from utils import tagged_streams

class qa_ts_insert_zeros (gr_unittest.TestCase):

    def test_one(self):
        n_packets = 10
        packet_length = 1000
        packets = [[i]*packet_length for i in range(1, n_packets+1)]
        tagname = "packet_length"
        data, tags = tagged_streams.packets_to_vectors(packets, tagname)
        tb = gr.top_block()
        src = gr.vector_source_c(data, False, 1, tags)
        rate_in = 16000
        rate_out = 48000
        ratio = float(rate_out) / rate_in
        throttle1 = blocks.throttle(gr.sizeof_gr_complex, rate_in)
        insert_zeros = digital.ts_insert_zeros_cc(tagname)
        throttle2 = blocks.throttle(gr.sizeof_gr_complex, rate_out)
        head = gr.head(gr.sizeof_gr_complex, int(n_packets * packet_length * ratio*2))
        snk = gr.vector_sink_c()
        tb.connect(src, throttle1, insert_zeros, throttle2, head, snk)
        tb.run()
        data = snk.data()
        state = 1
        pos = 0
        last_non_zero = 0
        for i, d in enumerate(data):
            if d != 0:
                last_non_zero = i
            if pos == 0:
                if (d == state):
                    pos = pos + 1
                elif (d != 0):
                    raise ValueError("Invalid")
            elif pos > 0:
                if (d != state):
                    raise ValueError("Invalid")
                pos = pos + 1
                if pos == packet_length:
                    state += 1
                    pos = 0
        min_ratio = ratio-1
        max_ratio = ratio+1
        self.assertEqual(state-1, n_packets)
        self.assertTrue(last_non_zero > min_ratio*packet_length*n_packets)
        self.assertTrue(last_non_zero < max_ratio*packet_length*n_packets)

if __name__ == '__main__':
    gr_unittest.run(qa_ts_insert_zeros, "qa_ts_insert_zeros.xml")
