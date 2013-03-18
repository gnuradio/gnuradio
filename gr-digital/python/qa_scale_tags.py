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

class qa_scale_tags (gr_unittest.TestCase):

    def test_utils(self):
        packets = ((1, 2, 3), (4, 5, 6, 7, 8), (9, 10))
        tagname = "vector_length"
        data, tags = tagged_streams.packets_to_vectors(packets, tagname)
        new_packets = tagged_streams.vectors_to_packets(data, tags, tagname)
        for np, op in zip(new_packets, packets):
            for n, o in zip(np, op):
                self.assertEqual(n, o)

    def test_001_t (self):
        packets = ((1, 2, 3), (4, 5, 6, 7, 8), (9, 10))
        tagname = "packet_length"
        data, tags = tagged_streams.packets_to_vectors(packets, tagname)
        tb = gr.top_block()
        src = gr.vector_source_b(data, False, 1, tags)
        tag_scaler = digital.scale_tags(1, tagname, 2)
        unpacker = blocks.packed_to_unpacked_bb(4, blocks.GR_MSB_FIRST)
        snk = gr.vector_sink_b()
        tb.connect(src, unpacker, tag_scaler, snk)
        tb.run()
        packets = tagged_streams.vectors_to_packets(snk.data(), snk.tags(), tagname)

if __name__ == '__main__':
    gr_unittest.run(qa_scale_tags, "qa_scale_tags.xml")
