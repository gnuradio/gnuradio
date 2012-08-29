#!/usr/bin/env python
#
# Copyright 2004,2005,2007,2010,2012 Free Software Foundation, Inc.
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
import blocks_swig

class test_stream_mux (gr_unittest.TestCase):

    def setUp (self):
        self.tb = gr.top_block ()

    def tearDown (self):
        self.tb = None

    def help_stream_2ff(self, N, stream_sizes):
        v0 = gr.vector_source_f(N*[1,], False)
        v1 = gr.vector_source_f(N*[2,], False)

        mux = blocks_swig.stream_mux(gr.sizeof_float, stream_sizes)

        dst = gr.vector_sink_f ()

        self.tb.connect (v0, (mux,0))
        self.tb.connect (v1, (mux,1))
        self.tb.connect (mux, dst)
        self.tb.run ()

        return dst.data ()

    def help_stream_ramp_2ff(self, N, stream_sizes):
        r1 = range(N)
        r2 = range(N)
        r2.reverse()

        v0 = gr.vector_source_f(r1, False)
        v1 = gr.vector_source_f(r2, False)

        mux = blocks_swig.stream_mux(gr.sizeof_float, stream_sizes)

        dst = gr.vector_sink_f ()

        self.tb.connect (v0, (mux,0))
        self.tb.connect (v1, (mux,1))
        self.tb.connect (mux, dst)
        self.tb.run ()

        return dst.data ()

    def test_stream_2NN_ff(self):
        N = 40
        stream_sizes = [10, 10]
        result_data = self.help_stream_2ff(N, stream_sizes)

        exp_data = (1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0,
                    2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0,
                    1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0,
                    2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0,
                    1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0,
                    2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0,
                    1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0,
                    2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0)
        self.assertEqual (exp_data, result_data)

    def test_stream_ramp_2NN_ff(self):
        N = 40
        stream_sizes = [10, 10]
        result_data = self.help_stream_ramp_2ff(N, stream_sizes)

        exp_data = ( 0.0,  1.0,  2.0,  3.0,  4.0,  5.0,  6.0,  7.0,  8.0,  9.0,
                    39.0, 38.0, 37.0, 36.0, 35.0, 34.0, 33.0, 32.0, 31.0, 30.0,
                    10.0, 11.0, 12.0, 13.0, 14.0, 15.0, 16.0, 17.0, 18.0, 19.0,
                    29.0, 28.0, 27.0, 26.0, 25.0, 24.0, 23.0, 22.0, 21.0, 20.0,
                    20.0, 21.0, 22.0, 23.0, 24.0, 25.0, 26.0, 27.0, 28.0, 29.0,
                    19.0, 18.0, 17.0, 16.0, 15.0, 14.0, 13.0, 12.0, 11.0, 10.0,
                    30.0, 31.0, 32.0, 33.0, 34.0, 35.0, 36.0, 37.0, 38.0, 39.0,
                     9.0,  8.0,  7.0,  6.0,  5.0,  4.0,  3.0,  2.0,  1.0,  0.0)
        self.assertEqual (exp_data, result_data)

    def test_stream_2NM_ff(self):
        N = 40
        stream_sizes = [7, 9]
        self.help_stream_2ff(N, stream_sizes)

        result_data = self.help_stream_2ff(N, stream_sizes)

        exp_data = (1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0,
                    2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0,
                    1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0,
                    2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0,
                    1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0,
                    2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0,
                    1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0,
                    2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0,
                    1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0,
                    2.0, 2.0, 2.0, 2.0)

        self.assertEqual (exp_data, result_data)


    def test_stream_2MN_ff(self):
        N = 37
        stream_sizes = [7, 9]
        self.help_stream_2ff(N, stream_sizes)

        result_data = self.help_stream_2ff(N, stream_sizes)

        exp_data = (1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0,
                    2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0,
                    1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0,
                    2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0,
                    1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0,
                    2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0,
                    1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0,
                    2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0,
                    1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0,
                    2.0)

        self.assertEqual (exp_data, result_data)

    def test_stream_2N0_ff(self):
        N = 30
        stream_sizes = [7, 0]
        self.help_stream_2ff(N, stream_sizes)

        result_data = self.help_stream_2ff(N, stream_sizes)

        exp_data = (1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0,
                    1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0,
                    1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0,
                    1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0,
                    1.0, 1.0)

        self.assertEqual (exp_data, result_data)

    def test_stream_20N_ff(self):
        N = 30
        stream_sizes = [0, 9]
        self.help_stream_2ff(N, stream_sizes)

        result_data = self.help_stream_2ff(N, stream_sizes)

        exp_data = (2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0,
                    2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0,
                    2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0,
                    2.0, 2.0, 2.0)

        self.assertEqual (exp_data, result_data)

if __name__ == '__main__':
    gr_unittest.run(test_stream_mux, "test_stream_mux.xml")
