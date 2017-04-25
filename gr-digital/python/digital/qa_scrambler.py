#!/usr/bin/env python
#
# Copyright 2008,2010,2012,2013 Free Software Foundation, Inc.
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

from gnuradio import gr, gr_unittest, digital, blocks
import pmt

# See gr-digital/lib/additive_scrambler_bb_impl.cc for reference.
def additive_scramble_lfsr(mask, seed, reglen, bpb, data):
    l = digital.lfsr(mask, seed, reglen)
    out = []
    for d in data:
        scramble_word = 0
        for i in xrange(0,bpb):
            scramble_word ^= l.next_bit() << i
        out.append(d ^ scramble_word)
    return tuple(out)

class test_scrambler(gr_unittest.TestCase):

    def setUp(self):
        self.tb = gr.top_block()

    def tearDown(self):
        self.tb = None

    def test_scrambler_descrambler(self):
        src_data = (1,)*1000
        src = blocks.vector_source_b(src_data, False)
        scrambler = digital.scrambler_bb(0x8a, 0x7F, 7)     # CCSDS 7-bit scrambler
        descrambler = digital.descrambler_bb(0x8a, 0x7F, 7)
        dst = blocks.vector_sink_b()
        self.tb.connect(src, scrambler, descrambler, dst)
        self.tb.run()
        self.assertEqual(tuple(src_data[:-8]), dst.data()[8:]) # skip garbage during synchronization

    def test_additive_scrambler(self):
        src_data = (1,)*1000
        src = blocks.vector_source_b(src_data, False)
        scrambler = digital.additive_scrambler_bb(0x8a, 0x7f, 7)
        descrambler = digital.additive_scrambler_bb(0x8a, 0x7f, 7)
        dst = blocks.vector_sink_b()
        self.tb.connect(src, scrambler, descrambler, dst)
        self.tb.run()
        self.assertEqual(src_data, dst.data())

    def test_additive_scrambler_reset(self):
        src_data = (1,)*1000
        src = blocks.vector_source_b(src_data, False)
        scrambler = digital.additive_scrambler_bb(0x8a, 0x7f, 7, 100)
        descrambler = digital.additive_scrambler_bb(0x8a, 0x7f, 7, 100)
        dst = blocks.vector_sink_b()
        self.tb.connect(src, scrambler, descrambler, dst)
        self.tb.run()
        self.assertEqual(src_data, dst.data())

    def test_additive_scrambler_reset_3bpb(self):
        src_data = (5,)*2000
        src = blocks.vector_source_b(src_data, False)
        scrambler = digital.additive_scrambler_bb(0x8a, 0x7f, 7, 100, 3)
        descrambler = digital.additive_scrambler_bb(0x8a, 0x7f, 7, 100, 3)
        dst = blocks.vector_sink_b()
        dst2 = blocks.vector_sink_b()
        self.tb.connect(src, scrambler, descrambler, dst)
        self.tb.connect(scrambler, dst2)
        self.tb.run()
        if not (src_data == dst.data()):
            self.fail('Not equal.')
        self.assertEqual(src_data, src_data)

    def test_additive_scrambler_tags(self):
        src_data = (1,)*1000
        src = blocks.vector_source_b(src_data, False)
        scrambler = digital.additive_scrambler_bb(0x8a, 0x7f, 7, 100)
        descrambler = digital.additive_scrambler_bb(0x8a, 0x7f, 7, 100)
        reset_tag_key = 'reset_lfsr'
        reset_tag1 = gr.tag_t()
        reset_tag1.key = pmt.string_to_symbol(reset_tag_key)
        reset_tag1.offset = 17
        reset_tag2 = gr.tag_t()
        reset_tag2.key = pmt.string_to_symbol(reset_tag_key)
        reset_tag2.offset = 110
        reset_tag3 = gr.tag_t()
        reset_tag3.key = pmt.string_to_symbol(reset_tag_key)
        reset_tag3.offset = 523
        src = blocks.vector_source_b(src_data, False, 1, (reset_tag1, reset_tag2, reset_tag3))
        scrambler = digital.additive_scrambler_bb(0x8a, 0x7f, 7, 100, 1, reset_tag_key)
        descrambler = digital.additive_scrambler_bb(0x8a, 0x7f, 7, 100, 1, reset_tag_key)
        dst = blocks.vector_sink_b()
        self.tb.connect(src, scrambler, descrambler, dst)
        self.tb.run()
        self.assertEqual(src_data, dst.data())

    def test_additive_scrambler_tags_oneway(self):
        src_data = range(0, 10)
        reset_tag_key = 'reset_lfsr'
        reset_tag1 = gr.tag_t()
        reset_tag1.key = pmt.string_to_symbol(reset_tag_key)
        reset_tag1.offset = 0
        reset_tag2 = gr.tag_t()
        reset_tag2.key = pmt.string_to_symbol(reset_tag_key)
        reset_tag2.offset = 10
        reset_tag3 = gr.tag_t()
        reset_tag3.key = pmt.string_to_symbol(reset_tag_key)
        reset_tag3.offset = 20
        src = blocks.vector_source_b(src_data * 3, False, 1, (reset_tag1, reset_tag2, reset_tag3))
        scrambler = digital.additive_scrambler_bb(0x8a, 0x7f, 7, 0, 8, reset_tag_key)
        dst = blocks.vector_sink_b()
        self.tb.connect(src, scrambler, dst)
        self.tb.run()
        expected_data = additive_scramble_lfsr(0x8a, 0x7f, 7, 8, src_data)
        self.assertEqual(expected_data * 3, dst.data())

if __name__ == '__main__':
    gr_unittest.run(test_scrambler, "test_scrambler.xml")
