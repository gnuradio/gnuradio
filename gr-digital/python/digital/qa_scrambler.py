#!/usr/bin/env python
#
# Copyright 2008,2010,2012,2013 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#
#

from gnuradio import gr, gr_unittest, digital, blocks
from gnuradio.digital.utils import lfsr_args
import numpy as np
import pmt

# See gr-digital/lib/additive_scrambler_bb_impl.cc for reference.


def additive_scramble_lfsr(mask, seed, reglen, bpb, data):
    l = digital.lfsr(mask, seed, reglen)
    out = []
    for d in data:
        scramble_word = 0
        for i in range(0, bpb):
            scramble_word ^= l.next_bit() << i
        out.append(d ^ scramble_word)
    return out

class test_scrambler(gr_unittest.TestCase):

    def setUp(self):
        self.tb = gr.top_block()

    def tearDown(self):
        self.tb = None


    def test_lfsr_002(self):
        _a = lfsr_args(1,51,3,0)
        l = digital.lfsr(*_a)
        seq = [l.next_bit() for _ in range(2**10)]
        reg = np.zeros(52,np.int8)
        reg[::-1][(51,3,0),] = 1
        res = (np.convolve(seq,reg)%2)
        self.assertTrue(sum(res[52:-52])==0,"LRS not generated properly")

    def test_scrambler_descrambler_001(self):
        src_data = np.random.randint(0,2,500,dtype=np.int8)
        src = blocks.vector_source_b(src_data, False)
        scrambler = digital.scrambler_bb(*lfsr_args(0b1,7,2,0))     # p(x) = x^7 + x^2 + 1
        descrambler = digital.descrambler_bb(*lfsr_args(0b111,7,2,0)) # we can use any seed here, it is descrambling.
        m_tap = blocks.vector_sink_b()
        dst = blocks.vector_sink_b()
        self.tb.connect(src, scrambler, descrambler, dst)
        self.tb.connect(scrambler, m_tap)
        self.tb.run()
        self.assertEqual(src_data[:-7].tolist(), dst.data()[7:]) # skip garbage during synchronization
        self.assertEqual(tuple(np.convolve(m_tap.data(),[1,0,0,0,0,1,0,1])%2)[7:-10],
                tuple(src_data[:-10])) # manual descrambling test

    def test_scrambler_descrambler_002(self):
        _a = lfsr_args(0b1,51,6,0) #p(x) = x^51+x^6+1
        src_data = np.random.randint(0,2,1000,dtype=np.int8)
        src = blocks.vector_source_b(src_data, False)
        scrambler = digital.scrambler_bb(*_a)
        m_tap = blocks.vector_sink_b()
        descrambler = digital.descrambler_bb(*_a)
        dst = blocks.vector_sink_b()
        self.tb.connect(src, scrambler, descrambler, dst)
        self.tb.connect(scrambler, m_tap)
        self.tb.run()
        self.assertTrue(np.all(src_data[:-51]==dst.data()[51:])) # skip garbage during synchronization
        reg = np.zeros(52,np.int8)
        reg[::-1][(51,6,0),] = 1
        self.assertTrue(np.all( np.convolve(m_tap.data(),reg)[51:-60]%2 
            == src_data[:-60])) # manual descrambling test

    def test_scrambler_descrambler_003(self):
        src_data = np.random.randint(0,2,1000,dtype=np.int8)
        src = blocks.vector_source_b(src_data, False)
        scrambler = digital.scrambler_bb(*lfsr_args(1,12,10,3,2,0)) # this is the product of the other two
        descrambler1 = digital.descrambler_bb(*lfsr_args(1,5,3,0))
        descrambler2 = digital.descrambler_bb(*lfsr_args(1,7,2,0))
        dst = blocks.vector_sink_b()
        self.tb.connect(src, scrambler, descrambler1, descrambler2, dst)
        self.tb.run()
        self.assertTrue(np.all(src_data[:-12]==dst.data()[12:])) # skip garbage during synchronization

    def test_additive_scrambler_001(self):
        _a = lfsr_args(1,51,3,0) #i p(x) = x^51+x^3+1, seed 0x1
        src_data = np.random.randint(0,2,1000,dtype=np.int8).tolist()
        src = blocks.vector_source_b(src_data, False)
        scrambler = digital.additive_scrambler_bb(*_a)
        descrambler = digital.additive_scrambler_bb(*_a)
        dst = blocks.vector_sink_b()
        self.tb.connect(src, scrambler, descrambler, dst)
        self.tb.run()
        self.assertEqual(tuple(src_data), tuple(dst.data()))

    def test_additive_scrambler_002(self):
        _a = lfsr_args(1,51,3,0) #i p(x) = x^51+x^3+1, seed 0x1
        src_data = [1,]*1000
        src = blocks.vector_source_b(src_data, False)
        scrambler = digital.additive_scrambler_bb(*_a)
        dst = blocks.vector_sink_b()
        self.tb.connect(src, scrambler, dst)
        self.tb.run()
        reg = np.zeros(52,np.int8)
        reg[::-1][(51,3,0),] = 1
        res = (np.convolve(dst.data(),reg)%2)[52:-52]
        self.assertEqual(len(res), sum(res)) # when convolved with mask, 
                                             # after sync, only 1's would be returned.

    def test_scrambler_descrambler(self):
        src_data = [1, ] * 1000
        src = blocks.vector_source_b(src_data, False)
        scrambler = digital.scrambler_bb(
            0x8a, 0x7F, 7)     # CCSDS 7-bit scrambler
        descrambler = digital.descrambler_bb(0x8a, 0x7F, 7)
        dst = blocks.vector_sink_b()
        self.tb.connect(src, scrambler, descrambler, dst)
        self.tb.run()
        # skip garbage during synchronization
        self.assertEqual(src_data[:-8], dst.data()[8:])

    def test_additive_scrambler(self):
        src_data = [1, ] * 1000
        src = blocks.vector_source_b(src_data, False)
        scrambler = digital.additive_scrambler_bb(0x8a, 0x7f, 7)
        descrambler = digital.additive_scrambler_bb(0x8a, 0x7f, 7)
        dst = blocks.vector_sink_b()
        self.tb.connect(src, scrambler, descrambler, dst)
        self.tb.run()
        self.assertEqual(src_data, dst.data())

    def test_additive_scrambler_reset(self):
        src_data = [1, ] * 200
        src = blocks.vector_source_b(src_data, False)
        scrambler = digital.additive_scrambler_bb(0x8a, 0x7f, 7, 50)
        dst = blocks.vector_sink_b()
        self.tb.connect(src, scrambler, dst)
        self.tb.run()
        output = dst.data()
        self.assertEqual(output[:50] * 4, output)

    def test_additive_scrambler_reset_3bpb(self):
        src_data = [5, ] * 200
        src = blocks.vector_source_b(src_data, False)
        scrambler = digital.additive_scrambler_bb(0x8a, 0x7f, 7, 50, 3)
        dst = blocks.vector_sink_b()
        self.tb.connect(src, scrambler, dst)
        self.tb.run()
        output = dst.data()
        self.assertEqual(output[:50] * 4, output)

    def test_additive_scrambler_tags(self):
        src_data = [1, ] * 1000
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
        src = blocks.vector_source_b(
            src_data, False, 1, (reset_tag1, reset_tag2, reset_tag3))
        scrambler = digital.additive_scrambler_bb(
            0x8a, 0x7f, 7, 100, 1, reset_tag_key)
        descrambler = digital.additive_scrambler_bb(
            0x8a, 0x7f, 7, 100, 1, reset_tag_key)
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
        src = blocks.vector_source_b(
            src_data * 3, False, 1, (reset_tag1, reset_tag2, reset_tag3))
        scrambler = digital.additive_scrambler_bb(
            0x8a, 0x7f, 7, 0, 8, reset_tag_key)
        dst = blocks.vector_sink_b()
        self.tb.connect(src, scrambler, dst)
        self.tb.run()
        expected_data = additive_scramble_lfsr(0x8a, 0x7f, 7, 8, src_data)
        self.assertEqual(expected_data * 3, dst.data())


if __name__ == '__main__':
    gr_unittest.run(test_scrambler)
