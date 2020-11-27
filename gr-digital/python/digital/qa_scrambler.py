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
import numpy as np
import pmt

# See gr-digital/lib/additive_scrambler_bb_impl.cc for reference.
def additive_scramble_lfsr(mask, seed, reglen, bpb, data):
    l = digital.lfsr(mask, seed, reglen)
    out = []
    for d in data:
        scramble_word = 0
        for i in range(0,bpb):
            scramble_word ^= l.next_bit() << i
        out.append(d ^ scramble_word)
    return tuple(out)

def make_mask(*exp):
    from functools import reduce
    return reduce(int.__xor__,map(lambda x:2**x,exp)),max(exp)-1

class test_scrambler(gr_unittest.TestCase):

    def setUp(self):
        self.tb = gr.top_block()

    def tearDown(self):
        self.tb = None

    def test_scrambler_descrambler(self):
        src_data = np.random.randint(0,2,500,dtype=np.int8)
        src = blocks.vector_source_b(src_data, False)
        scrambler = digital.scrambler_bb(0x85, 1, 6)     # p(x) = x^7 + x^2 + 1
        dst_tap = blocks.vector_sink_b()
        descrambler = digital.descrambler_bb(0x85, 0b111, 6) # we can use seed 0 here, it is descrambling.
        dst = blocks.vector_sink_b()
        self.tb.connect(src, scrambler, descrambler, dst)
        self.tb.connect(scrambler, dst_tap)
        self.tb.run()
        self.assertEqual(tuple(src_data[:-7]), dst.data()[7:]) # skip garbage during synchronization
        self.assertEqual(tuple(np.convolve(dst_tap.data(),[1,0,0,0,0,1,0,1])%2)[7:-10],
                tuple(src_data[:-10])) # manual descrambling test

    def test_scrambler_descrambler_big(self):
        mask,k = make_mask(51,6,0) #p(x) = x^51+x^6+1
        src_data = np.random.randint(0,2,1000,dtype=np.int8)
        src = blocks.vector_source_b(src_data, False)
        scrambler = digital.scrambler_bb(mask, 1, k)     # p(x) = x^7 + x^2 + 1
        dst_tap = blocks.vector_sink_b()
        descrambler = digital.descrambler_bb(mask, 0b111, k) # we can use seed 0 here, it is descrambling.
        dst = blocks.vector_sink_b()
        self.tb.connect(src, scrambler, descrambler, dst)
        self.tb.connect(scrambler, dst_tap)
        self.tb.run()
        self.assertTrue(np.all(src_data[:-51]==dst.data()[51:])) # skip garbage during synchronization
        reg = np.zeros(52,np.int8)
        reg[::-1][(51,6,0),] = 1
        self.assertTrue(np.all( np.convolve(dst_tap.data(),reg)[51:-60]%2 
            == src_data[:-60])) # manual descrambling test

    def test_additive_scrambler(self):
        mask,k = make_mask(5,3,0) #p(x) = x^5+x^3+1, primitive, LRS of length 2**5-1
        lrslen = 2**5-1
        src_data = (1,)*lrslen*2
        src = blocks.vector_source_b(src_data, False)
        scrambler = digital.additive_scrambler_bb(mask, 1, k)
        dst_tap = blocks.vector_sink_b()
        descrambler = digital.additive_scrambler_bb(mask, 1, k)
        dst = blocks.vector_sink_b()
        self.tb.connect(src, scrambler, descrambler, dst)
        self.tb.connect(scrambler, dst_tap)
        self.tb.run()
        self.assertEqual(src_data, dst.data())
        tap_data = dst_tap.data()[:lrslen*2]
        self.assertEqual(tap_data[:lrslen],tap_data[lrslen:]) # The LRS should appear identical and twice
        res = (np.convolve(tap_data,[1,0,1,0,0,1])%2)[5:-5]
        self.assertEqual(len(res),sum(res)) # when convolved with mask, only 1's whould be returned.

    def test_additive_scrambler_big(self):
        mask,k = make_mask(51,3,0) #p(x) = x^51+x^3+1
        src_data = np.random.randint(0,2,1000,dtype=np.int8).tolist()
        src = blocks.vector_source_b(src_data, False)
        scrambler = digital.additive_scrambler_bb(mask, 1, k)
        descrambler = digital.additive_scrambler_bb(mask, 1, k)
        dst = blocks.vector_sink_b()
        self.tb.connect(src, scrambler, descrambler, dst)
        self.tb.run()
        self.assertEqual(tuple(src_data), tuple(dst.data()))

    def test_additive_scrambler_reset(self):
        src_data = (1,)*200
        src = blocks.vector_source_b(src_data, False)
        scrambler = digital.additive_scrambler_bb(0x8a, 0x7f, 7, 50)
        dst = blocks.vector_sink_b()
        self.tb.connect(src, scrambler, dst)
        self.tb.run()
        output = dst.data()
        self.assertEqual(output[:50] * 4, output)

    def test_additive_scrambler_reset_3bpb(self):
        src_data = (5,)*200
        src = blocks.vector_source_b(src_data, False)
        scrambler = digital.additive_scrambler_bb(0x8a, 0x7f, 7, 50, 3)
        dst = blocks.vector_sink_b()
        self.tb.connect(src, scrambler, dst)
        self.tb.run()
        output = dst.data()
        self.assertEqual(output[:50] * 4, output)

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
        src_data = [x for x in range(0, 10)]
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
