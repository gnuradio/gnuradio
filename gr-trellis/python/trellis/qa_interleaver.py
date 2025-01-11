#!/usr/bin/env python
#
# Copyright 2004,2010,2012 Free Software Foundation, Inc.
# Copyright 2025 Marcus Müller <mmueller@gnuradio.org>
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#
#


from gnuradio import gr_unittest, trellis


class test_interleaver(gr_unittest.TestCase):

    def test_001_interleaver(self):
        K = 5
        IN = [1, 2, 3, 4, 0]
        DIN = [4, 0, 1, 2, 3]
        i = trellis.interleaver(K, IN)
        self.assertEqual((K, IN, DIN), (i.K(), i.INTER(), i.DEINTER()))


if __name__ == "__main__":
    gr_unittest.run(test_interleaver)
