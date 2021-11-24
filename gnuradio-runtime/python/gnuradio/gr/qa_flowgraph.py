#!/usr/bin/env python
#
# Copyright 2016 Free Software Foundation, Inc.
# Copyright 2021 Marcus Müller
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#
#


from gnuradio import gr, gr_unittest


class test_flowgraph (gr_unittest.TestCase):

    def setUp(self):
        self.tb = gr.top_block()

    def tearDown(self):
        self.tb = None

    def test_000_empty_fg(self):
        self.tb.start()
        self.tb.stop()


if __name__ == '__main__':
    gr_unittest.run(test_flowgraph)
