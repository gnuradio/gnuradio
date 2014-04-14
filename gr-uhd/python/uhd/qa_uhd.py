#!/usr/bin/env python
#
# Copyright 2005,2008,2010 Free Software Foundation, Inc.
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

from gnuradio import gr, gr_unittest, uhd

class test_uhd(gr_unittest.TestCase):

    def setUp(self):
        self.tb = gr.top_block()

    def tearDown(self):
        self.tb = None

    def test_000_nop (self):
        """Just see if we can import the module...
        They may not have a UHD device connected, etc.  Don't try to run anything"""
        pass

    def test_time_spec_t (self):
        seconds = 42.0
        time = uhd.time_spec_t(seconds)
        twice_time = time + time;
        zero_time = time - time;
        self.assertEqual(time.get_real_secs() * 2,  seconds * 2 )
        self.assertEqual(time.get_real_secs() - time.get_real_secs() , 0.0)

    def test_stream_args_channel_foo(self):
        """
        Try to manipulate the stream args channels for proper swig'ing checks.
        """
        sa = uhd.stream_args_t()
        sa.channels.append(1)
        sa.channels.append(0)
        print sa.channels
        self.assertEqual(len(sa.channels), 2)
        self.assertEqual(sa.channels[0], 1)
        self.assertEqual(sa.channels[1], 0)

if __name__ == '__main__':
    gr_unittest.run(test_uhd, "test_uhd.xml")
