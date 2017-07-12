#!/usr/bin/env python
#
# Copyright 2013 Free Software Foundation, Inc.
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
        self.tb.connect(src, gate, sink)
        self.tb.run ()
        self.assertEqual(len(sink.tags()), 0)

    def test_002_t (self):
        tags = []
        tags.append(gr.tag_t())
        tags[0].key = pmt.string_to_symbol('key')
        tags[0].value = pmt.from_long(42)
        tags[0].offset = 0
        tags.append(gr.tag_t())
        tags[1].key = pmt.string_to_symbol('key')
        tags[1].value = pmt.from_long(42)
        tags[1].offset = 5
        tags.append(gr.tag_t())
        tags[2].key = pmt.string_to_symbol('secondkey')
        tags[2].value = pmt.from_long(42)
        tags[2].offset = 6
        src = blocks.vector_source_f(range(20), False, 1, tags)
        gate = blocks.tag_gate(gr.sizeof_float, False)
        gate.set_single_key("key")
        self.assertEqual(gate.single_key(),"key")
        sink = blocks.vector_sink_f()
        self.tb.connect(src, gate, sink)
        self.tb.run ()
        self.assertEqual(len(sink.tags()), 1)

    def test_003_t (self):
        tags = []
        tags.append(gr.tag_t())
        tags[0].key = pmt.string_to_symbol('key')
        tags[0].value = pmt.from_long(42)
        tags[0].offset = 0
        tags.append(gr.tag_t())
        tags[1].key = pmt.string_to_symbol('key')
        tags[1].value = pmt.from_long(42)
        tags[1].offset = 5
        tags.append(gr.tag_t())
        tags[2].key = pmt.string_to_symbol('secondkey')
        tags[2].value = pmt.from_long(42)
        tags[2].offset = 6
        src = blocks.vector_source_f(range(20), False, 1, tags)
        gate = blocks.tag_gate(gr.sizeof_float, True)
        gate.set_single_key("key")
        sink = blocks.vector_sink_f()
        self.tb.connect(src, gate, sink)
        self.tb.run ()
        self.assertEqual(len(sink.tags()), 3)

    def test_004_t (self):
        tags = []
        tags.append(gr.tag_t())
        tags[0].key = pmt.string_to_symbol('key')
        tags[0].value = pmt.from_long(42)
        tags[0].offset = 0
        tags.append(gr.tag_t())
        tags[1].key = pmt.string_to_symbol('key')
        tags[1].value = pmt.from_long(42)
        tags[1].offset = 5
        tags.append(gr.tag_t())
        tags[2].key = pmt.string_to_symbol('secondkey')
        tags[2].value = pmt.from_long(42)
        tags[2].offset = 6
        src = blocks.vector_source_f(range(20), False, 1, tags)
        gate = blocks.tag_gate(gr.sizeof_float, True)
        sink = blocks.vector_sink_f()
        self.tb.connect(src, gate, sink)
        self.tb.run ()
        self.assertEqual(len(sink.tags()), 3)

    def test_005_t (self):
        gate = blocks.tag_gate(gr.sizeof_float, True)
        self.assertEqual(gate.single_key(), "")
        gate.set_single_key("the_key")
        self.assertEqual(gate.single_key(), "the_key")
        gate.set_single_key("")
        self.assertEqual(gate.single_key(), "")

if __name__ == '__main__':
    gr_unittest.run(qa_tag_gate, "qa_tag_gate.xml")

