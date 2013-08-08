#!/usr/bin/env python
#
# Copyright 2004,2007,2010,2013 Free Software Foundation, Inc.
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

class test_gsm_vocoder (gr_unittest.TestCase):

    def setUp (self):
        self.tb = gr.top_block ()

    def tearDown (self):
        self.tb = None

    def test001_module_load (self):
        data = 20*(100,200,300,400,500,600,700,800)
        expected_data = (0,0,360,304,256,720,600,504,200,144,128,464,
                         376,384,680,576,440,264,176,176,640,520,480,
                         464,384,288,432,296,328,760,624,504,176,96,96,
                         416,312,360,808,672,216,104,136,504,376,448,
                         720,608,296,304,176,336,576,456,560,512,320,
                         216,344,264,456,672,576,488,192,80,152,424,
                         368,552,688,560,280,200,104,256,520,464,608,
                         488,184,104,16,472,456,360,696,568,208,136,88,
                         376,480,456,616,472,232,224,264,320,512,624,
                         632,520,176,80,192,304,400,592,664,552,248,152,
                         144,336,440,520,616,664,304,176,80,536,448,376,
                         680,600,240,168,112,408,488,472,608,480,240,232,
                         208,288,480,600,616,520,176,88,184,296,392,584,
                         656,552,248,160,144,336,432,512,608,664)
        src = blocks.vector_source_s(data)
        enc = vocoder.gsm_fr_encode_sp()
        dec = vocoder.gsm_fr_decode_ps()
        snk = blocks.vector_sink_s()
        self.tb.connect(src, enc, dec, snk)
        self.tb.run()
        actual_result = snk.data()
        self.assertEqual(expected_data, actual_result)

if __name__ == '__main__':
    gr_unittest.run(test_gsm_vocoder, "test_gsm_vocoder.xml")
