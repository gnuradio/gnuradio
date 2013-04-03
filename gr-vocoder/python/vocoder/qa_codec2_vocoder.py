#!/usr/bin/env python
#
# Copyright 2011,2013 Free Software Foundation, Inc.
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

from gnuradio import gr, gr_unittest, vocoder, blocks

class test_codec2_vocoder (gr_unittest.TestCase):

    def setUp (self):
        self.tb = gr.top_block()

    def tearDown (self):
        self.tb = None

    def test001_module_load (self):
        data = 20*(100,200,300,400,500,600,700,800)
        expected_data = (0,0,0,3,2,0,1,5,6,7,1,-1,0,-5,-11,-10,-20,-22,
                         -20,-20,-27,-26,-36,-48,-59,-24,5,-7,-12,-27,-22,
                         -22,-16,13,20,39,23,25,8,-6,15,44,97,135,145,125,
                         94,102,126,129,165,192,180,132,99,79,73,83,72,47,
                         40,0,-32,-46,-67,-99,-123,-114,-87,-108,-131,-152,
                         -181,-245,-348,-294,-101,-71,-85,-26,99,123,15,2,77,
                         13,-117,-145,-105,-39,-50,-89,-59,-77,-134,-95,-51,
                         -22,17,-19,-59,-74,-103,-78,4,77,113,60,18,13,-67,
                         -49,24,88,179,190,89,18,-90,-102,-50,-5,123,135,57,
                         31,-82,-98,-51,6,93,104,44,-5,-84,-107,-44,45,102,104,
                         15,-47,-107,-126,-87,-11,89,93,13,-95,-136,-187,-70,
                         -167,216,-70,-103,175,-284,-486)
        src = blocks.vector_source_s(data)
        enc = vocoder.codec2_encode_sp()
        dec = vocoder.codec2_decode_ps()
        snk = blocks.vector_sink_s()
        self.tb.connect(src, enc, dec, snk)
        self.tb.run()
        actual_result = snk.data()
        self.assertEqual(expected_data, actual_result)

if __name__ == '__main__':
    gr_unittest.run(test_codec2_vocoder, "test_codec2_vocoder.xml")
