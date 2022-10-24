#!/usr/bin/env python
#
# Copyright 2019 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#
#


from gnuradio import gr, gr_unittest, analog, blocks, streamops


class test_fm_deemph(gr_unittest.TestCase):

    def setUp(self):
        self.tb = gr.top_block()

    def tearDown(self):
        self.tb = None

    def test_002(self):
        # Confirm that we can instantiate and run an FM deemphasis block
        tb = self.tb

        src = analog.sig_source_f(48000, analog.waveform_t.COS, 5000.0, 1.0)
        op = analog.fm_deemph(fs=48000, tau=75e-6)
        head = streamops.head(100)
        dst = blocks.vector_sink_f()

        tb.connect(src, op)
        tb.connect(op, head)
        tb.connect(head, dst)
        tb.run()


if __name__ == '__main__':
    gr_unittest.run(test_fm_deemph)
