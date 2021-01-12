#!/usr/bin/env python
#
# Copyright 2005,2008,2010 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#
"""
gr-uhd sanity checking
"""

from gnuradio import gr, gr_unittest, uhd

class test_uhd(gr_unittest.TestCase):

    def setUp(self):
        self.tb = gr.top_block()

    def tearDown(self):
        self.tb = None

    def test_000_nop(self):
        """Just see if we can import the module...
        They may not have a UHD device connected, etc.  Don't try to run anything"""
        pass

    def test_time_spec_t(self):
        seconds = 42.0
        time = uhd.time_spec_t(seconds)
        twice_time = time + time
        zero_time = time - time
        self.assertEqual(time.get_real_secs() * 2, seconds * 2)
        self.assertEqual(time.get_real_secs() - time.get_real_secs(), 0.0)

    def test_stream_args_channel_foo(self):
        """
        Try to manipulate the stream args channels for proper swig'ing checks.
        """
        # FYI: stream_args_t.channels.append does not work due to copy operation of STL vectors
        sa = uhd.stream_args_t()
        sa.channels = [1, 0]
        print(sa.channels)
        self.assertEqual(len(sa.channels), 2)
        self.assertEqual(sa.channels[0], 1)
        self.assertEqual(sa.channels[1], 0)


if __name__ == '__main__':
    gr_unittest.run(test_uhd)
