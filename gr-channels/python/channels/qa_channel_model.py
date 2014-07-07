#!/usr/bin/env python
#
# Copyright 2012,2013 Free Software Foundation, Inc.
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

from gnuradio import gr, gr_unittest, analog, blocks, channels
import math

class test_channel_model(gr_unittest.TestCase):

    def setUp(self):
        self.tb = gr.top_block()

    def tearDown(self):
        self.tb = None

    def test_000(self):
        N = 1000         # number of samples to use
        fs = 1000        # baseband sampling rate
        freq = 100

        signal = analog.sig_source_c(fs, analog.GR_SIN_WAVE, freq, 1)
        head = blocks.head(gr.sizeof_gr_complex, N)
        op = channels.channel_model(0.0, 0.0, 1.0, [1,], 0)
        snk = blocks.vector_sink_c()
        snk1 = blocks.vector_sink_c()

        op.set_noise_voltage(0.0)
        op.set_frequency_offset(0.0)
        op.set_taps([1,])
        op.set_timing_offset(1.0)

        self.tb.connect(signal, head, op, snk)
        self.tb.connect(op, snk1)
        self.tb.run()

        dst_data = snk.data()
        exp_data = snk1.data()
        self.assertComplexTuplesAlmostEqual(exp_data, dst_data, 5)

if __name__ == '__main__':
    gr_unittest.run(test_channel_model, "test_channel_model.xml")
