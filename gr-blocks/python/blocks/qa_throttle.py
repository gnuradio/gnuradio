#!/usr/bin/env python
#
# Copyright 2013 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#
#


from gnuradio import gr, gr_unittest, blocks

class test_throttle(gr_unittest.TestCase):

    def setUp(self):
        self.tb = gr.top_block()

    def tearDown(self):
        self.tb = None

    def test_01(self):
        # Test that we can make the block
        op = blocks.throttle(gr.sizeof_gr_complex, 1)

if __name__ == '__main__':
    gr_unittest.run(test_throttle, "test_throttle.xml")
