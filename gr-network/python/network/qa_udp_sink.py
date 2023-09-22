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


class qa_udp_sink (gr_unittest.TestCase):
    def setUp(self):
        self.tb = gr.top_block()

    def tearDown(self):
        self.tb = None

    def test_restart(self):
        null_source = blocks.null_source(gr.sizeof_gr_complex)
        throttle = blocks.throttle(gr.sizeof_gr_complex, 320000, True)
        udp_sink = network.udp_sink(gr.sizeof_gr_complex, 1, '127.0.0.1', 2000,
                                    0, 1472, False)
        self.tb.connect(null_source, throttle, udp_sink)
        self.tb.start()
        time.sleep(0.1)
        self.tb.stop()
        time.sleep(0.1)
        self.tb.start()
        time.sleep(0.1)
        self.tb.stop()


if __name__ == '__main__':
    gr_unittest.run(qa_udp_sink)
