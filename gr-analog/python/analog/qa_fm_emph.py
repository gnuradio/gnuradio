#!/usr/bin/env python
#
# Copyright 2019 Free Software Foundation, Inc.
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


from gnuradio import gr, gr_unittest, analog, blocks


class test_fm_emph(gr_unittest.TestCase):

    def setUp(self):
        self.tb = gr.top_block()

    def tearDown(self):
        self.tb = None

    def test_001(self):
        # Confirm that we can instantiate and run an FM preemphasis block
        tb = self.tb

        src = analog.sig_source_f(48000, analog.GR_COS_WAVE, 5000.0, 1.0)
        op = analog.fm_preemph(fs=48000, tau=75e-6, fh=-1.0)
        head = blocks.head(gr.sizeof_float, 100)
        dst = blocks.vector_sink_f()

        tb.connect(src, op)
        tb.connect(op, head)
        tb.connect(head, dst)
        tb.run()

    def test_002(self):
        # Confirm that we can instantiate and run an FM deemphasis block
        tb = self.tb

        src = analog.sig_source_f(48000, analog.GR_COS_WAVE, 5000.0, 1.0)
        op = analog.fm_deemph(fs=48000, tau=75e-6)
        head = blocks.head(gr.sizeof_float, 100)
        dst = blocks.vector_sink_f()

        tb.connect(src, op)
        tb.connect(op, head)
        tb.connect(head, dst)
        tb.run()


if __name__ == '__main__':
    gr_unittest.run(test_fm_emph)
