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

class test_g721_vocoder (gr_unittest.TestCase):

    def setUp (self):
        self.tb = gr.top_block()

    def tearDown (self):
        self.tb = None

    def test001_module_load (self):
        data = (8,24,36,52,56,64,76,88,104,124,132,148,172,
                196,220,244,280,320,372,416,468,524,580,648)
        src = blocks.vector_source_s(data)
        enc = vocoder.g721_encode_sb()
        dec = vocoder.g721_decode_bs()
        snk = blocks.vector_sink_s()
        self.tb.connect(src, enc, dec, snk)
        self.tb.run()
        actual_result = snk.data()
        self.assertEqual(data, actual_result)

if __name__ == '__main__':
    gr_unittest.run(test_g721_vocoder, "test_g721_vocoder.xml")
