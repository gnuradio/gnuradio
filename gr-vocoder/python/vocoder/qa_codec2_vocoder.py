#!/usr/bin/env python
#
# Copyright 2011,2013,2014 Free Software Foundation, Inc.
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
from gnuradio.vocoder import codec2

class test_codec2_vocoder (gr_unittest.TestCase):

    def setUp (self):
        self.tb = gr.top_block()

    def tearDown (self):
        self.tb = None

    def test001_module_load (self):
        data = 40*(100,200,300,400,500,600,700,800)
        expected_data = (0, 5, 10, 14, 15, 13, 14, 20, 40, 46, 39, 36, 35, 33, 22, 17, 31, 34, 29, 24, 24, 15, -3, -8, -7, 1, -4, -11, -14, -22, -39, -53, -51, -52, -58, -58, -59, -58, -61, -74, -73, -79, -75, -61, -73, -76, -72, -75, -62, -74, -75, -64, -64, -59, -61, -49, -68, -60, -23, -46, -48, -33, -48, 2, 20, -3, 2, -8, 9, 38, 9, 16, 23, 16, 44, 65, 37, 24, 25, 55, 61, 57, 52, 39, 47, 57, 66, 73, 50, 46, 47, 55, 55, 45, 73, 86, 63, 66, 60, 55, 60, 55, 71, 59, 46, 58, 46, 2, 38, 50, 33, 41, 32, 0, -16, -11, 10, 16, -13, 0, -5, -33, -45, -38, -28, -24, -41, 21, -2, -53, -55, -74, -66, -64, -64, -41, -46, -94, -122, -130, -92, -126, -104, -90, -74, -118, -162, -154, -130, -133, -163, -18, -23, -155, -95, -145, -60, -63, 156, 864, 882, 607, 449, 163, 204, 17, 47, 612, 447, 200, -59, -188, -175, -418, -192, 170, 14, -73, -258, -276, -267, -335, -117, 96, 34, -28, -152, -130, -124, -187, 42, 176, 131, 78, -52, -2, -57, -75, 104, 130, 111, 29, -50, -46, -107, -64, 66, 36, 33, -39, -129, -91, -157, -39, 69, 1, -12, -84, -99, -52, -61, 86, 147, 58, 21, -63, -60, -100, -48, 68, 76, 6, -65, -79, -108, -159, -71, 89, 171, 183, 216, 152, 26, -35, 0, 87, 126, 143, 182, 151, 95, 106, 115, 155, 103, 86, 127, 12, -41, -91, -87, -32, -52, -41, -32, -123, -147, -154, -156, -61, -37, -8, -51, -127, -132, -127, -107, -54, 1, 26, -17, -100, -61, -9, 3, 57, 117, 102, 58, -47, 24, 67, 42, 116, 141, 113, 39, -15, 63, 68, 41, 118, 80, 24, -46, -72, 12, 5, -17, 18, -43, -61, -110, -119, -42, -40, -16, 2, -11, -50)

        src = blocks.vector_source_s(data)
        enc = vocoder.codec2_encode_sp(codec2.MODE_2400)
        dec = vocoder.codec2_decode_ps(codec2.MODE_2400)
        snk = blocks.vector_sink_s()
        self.tb.connect(src, enc, dec, snk)
        self.tb.run()
        actual_result = snk.data()
        self.assertEqual(expected_data, actual_result)
        self.tb.disconnect(src, enc, dec, snk)

if __name__ == '__main__':
    # Note: The Vocoder is stateful, which means this test will produce failure when removing the xml option.
    # Perhaps this is not the best way to test such a vocoder.
    gr_unittest.run(test_codec2_vocoder, "test_codec2_vocoder.xml")
