#!/usr/bin/env python
#
# Copyright 2008,2010 Free Software Foundation, Inc.
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

from gnuradio import gr, gr_unittest

class test_scrambler(gr_unittest.TestCase):

    def setUp (self):
        self.tb = gr.top_block()

    def tearDown(self):
        self.tb = None

    def test_scrambler_descrambler(self):
        src_data = (1,)*1000
        src = gr.vector_source_b(src_data, False)
        scrambler = gr.scrambler_bb(0x8a, 0x7F, 7)     # CCSDS 7-bit scrambler
        descrambler = gr.descrambler_bb(0x8a, 0x7F, 7) 
        dst = gr.vector_sink_b()
        self.tb.connect(src, scrambler, descrambler, dst)
        self.tb.run()
        self.assertEqual(tuple(src_data[:-8]), dst.data()[8:]) # skip garbage during synchronization

    def test_additive_scrambler(self):
        src_data = (1,)*1000
        src = gr.vector_source_b(src_data, False)
        scrambler = gr.additive_scrambler_bb(0x8a, 0x7f, 7)
        descrambler = gr.additive_scrambler_bb(0x8a, 0x7f, 7)
        dst = gr.vector_sink_b()
        self.tb.connect(src, scrambler, descrambler, dst)
        self.tb.run()
        self.assertEqual(src_data, dst.data())                            

    def test_additive_scrambler_reset(self):
        src_data = (1,)*1000
        src = gr.vector_source_b(src_data, False)
        scrambler = gr.additive_scrambler_bb(0x8a, 0x7f, 7, 100)
        descrambler = gr.additive_scrambler_bb(0x8a, 0x7f, 7, 100)
        dst = gr.vector_sink_b()
        self.tb.connect(src, scrambler, descrambler, dst)
        self.tb.run()
        self.assertEqual(src_data, dst.data())                            

if __name__ == '__main__':
    gr_unittest.run(test_scrambler, "test_scrambler.xml")
