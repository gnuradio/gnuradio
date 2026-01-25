#!/usr/bin/env python
#
# Copyright 2026 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#

from gnuradio import gr, gr_unittest


class test_realtime(gr_unittest.TestCase):
    def test_000_enable_realtime(self):
        """Ensure this function is callable."""
        gr.enable_realtime_scheduling()


if __name__ == "__main__":
    gr_unittest.run(test_realtime)

