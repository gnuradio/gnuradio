#!/usr/bin/env python
# -*- coding: utf-8 -*-
#
# Copyright 2014 Free Software Foundation, Inc.
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

import pmt
from gnuradio import gr, gr_unittest
from gnuradio import blocks

class qa_tsb_vector_sink (gr_unittest.TestCase):

    def setUp (self):
        self.tb = gr.top_block ()
        self.tsb_key = "tsb"

    def tearDown (self):
        self.tb = None

    def test_001_t (self):
        packet_len = 4
        data = range(2 * packet_len)
        tag = gr.tag_t()
        tag.key = pmt.intern("foo")
        tag.offset = 5
        tag.value = pmt.intern("bar")
        src = blocks.vector_source_f(data, tags=(tag,))
        sink = blocks.tsb_vector_sink_f(tsb_key=self.tsb_key)
        self.tb.connect(
                src,
                blocks.stream_to_tagged_stream(gr.sizeof_float, 1, packet_len, self.tsb_key),
                sink
        )
        self.tb.run()
        self.assertEqual((tuple(data[0:packet_len]), tuple(data[packet_len:])), sink.data())
        self.assertEqual(len(sink.tags()), 1)
        self.assertEqual(sink.tags()[0].offset, tag.offset)


if __name__ == '__main__':
    gr_unittest.run(qa_tsb_vector_sink, "qa_tsb_vector_sink.xml")
