#!/usr/bin/env python
#
# Copyright 2004,2007,2010,2011 Free Software Foundation, Inc.
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
import digital_swig
import math

class test_constellation_decoder (gr_unittest.TestCase):

    def setUp (self):
        self.tb = gr.top_block ()

    def tearDown (self):
        self.tb = None

    def test_constellation_decoder_cb_bpsk (self):
        cnst = digital_swig.constellation_bpsk()
  	src_data =        (0.5 + 0.5j,  0.1 - 1.2j, -0.8 - 0.1j, -0.45 + 0.8j,
                           0.8 + 1.0j, -0.5 + 0.1j,  0.1 - 1.2j)
	expected_result = (        1,           1,           0,            0,
                                   1,           0,           1)
        src = gr.vector_source_c (src_data)
        op = digital_swig.constellation_decoder_cb (cnst.base())
        dst = gr.vector_sink_b ()

        self.tb.connect (src, op)
        self.tb.connect (op, dst)
        self.tb.run ()               # run the graph and wait for it to finish

        actual_result = dst.data ()  # fetch the contents of the sink
	#print "actual result", actual_result
	#print "expected result", expected_result
        self.assertFloatTuplesAlmostEqual (expected_result, actual_result)

    def test_constellation_decoder_cb_qpsk (self):
        cnst = digital_swig.constellation_qpsk()
  	src_data =        (0.5 + 0.5j,  0.1 - 1.2j, -0.8 - 0.1j, -0.45 + 0.8j,
                           0.8 + 1.0j, -0.5 + 0.1j,  0.1 - 1.2j)
	expected_result = (        3,           1,           0,            2,
                                   3,           2,           1)
        src = gr.vector_source_c (src_data)
        op = digital_swig.constellation_decoder_cb (cnst.base())
        dst = gr.vector_sink_b ()

        self.tb.connect (src, op)
        self.tb.connect (op, dst)
        self.tb.run ()               # run the graph and wait for it to finish

        actual_result = dst.data ()  # fetch the contents of the sink
	#print "actual result", actual_result
	#print "expected result", expected_result
        self.assertFloatTuplesAlmostEqual (expected_result, actual_result)


if __name__ == '__main__':
    gr_unittest.run(test_constellation_decoder, "test_constellation_decoder.xml")

