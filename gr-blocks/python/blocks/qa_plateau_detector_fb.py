#!/usr/bin/env python
#
# Copyright 2012-2013 Free Software Foundation, Inc.
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

class qa_plateau_detector_fb (gr_unittest.TestCase):

    def setUp (self):
        self.tb = gr.top_block ()

    def tearDown (self):
        self.tb = None

    def test_001_t (self):
        #                  | Spur spike 1     | Plateau                  | Spur spike 2
        test_signal  = (0, 1, .2, .4, .6, .8, 1, 1, 1, 1, 1, .8, .6, .4, 1, 0)
        expected_sig = (0, 0,  0,  0,  0,  0, 0, 0, 1, 0, 0,  0,  0,  0, 0, 0)
        #                                           | Center of Plateau
        sink = blocks.vector_sink_b()
        self.tb.connect(blocks.vector_source_f(test_signal), blocks.plateau_detector_fb(5), sink)
        self.tb.run ()
        self.assertEqual(expected_sig, sink.data())


if __name__ == '__main__':
    gr_unittest.run(qa_plateau_detector_fb, "qa_plateau_detector_fb.xml")
