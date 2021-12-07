#!/usr/bin/env python
#
# Copyright 2021 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#

from gnuradio import gr, gr_unittest, blocks, network
import time


class qa_udp_source (gr_unittest.TestCase):
    def setUp(self):
        self.tb = gr.top_block()

    def tearDown(self):
        self.tb = None

    def test_restart(self):
        udp_source = network.udp_source(gr.sizeof_gr_complex, 1, 1234, 0, 1472,
                                        False, False, False)
        null_sink = blocks.null_sink(gr.sizeof_gr_complex)
        self.tb.connect(udp_source, null_sink)
        self.tb.start()
        time.sleep(0.1)
        self.tb.stop()
        time.sleep(0.1)
        self.tb.start()
        time.sleep(0.1)
        self.tb.stop()


if __name__ == '__main__':
    gr_unittest.run(qa_udp_source)
