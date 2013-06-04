#!/usr/bin/env python
# 
# Copyright 2013 <+YOU OR YOUR COMPANY+>.
# 
# This is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3, or (at your option)
# any later version.
# 
# This software is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with this software; see the file COPYING.  If not, write to
# the Free Software Foundation, Inc., 51 Franklin Street,
# Boston, MA 02110-1301, USA.
# 

from gnuradio import gr, gr_unittest, blocks
import pmt


class qa_tag_gate (gr_unittest.TestCase):

    def setUp (self):
        self.tb = gr.top_block ()

    def tearDown (self):
        self.tb = None

    def test_001_t (self):
        tag = gr.tag_t()
        tag.key = pmt.string_to_symbol('key')
        tag.value = pmt.from_long(42)
        tag.offset = 0
        src = blocks.vector_source_f(range(20), False, 1, (tag,))
        gate = blocks.tag_gate(gr.sizeof_float, False)
        sink = blocks.vector_sink_f()
        self.tb.run ()
        self.assertEqual(len(sink.tags()), 0)

if __name__ == '__main__':
    gr_unittest.run(qa_tag_gate, "qa_tag_gate.xml")

