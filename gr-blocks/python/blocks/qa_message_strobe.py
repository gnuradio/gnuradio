#!/usr/bin/env python
# -*- coding: utf-8 -*-
#
# Copyright 2021 Free Software Foundation, Inc.
#
# This is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3, or (at your option)
# any later version.
#
# This software is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this software; see the file COPYING.  If not, write to
# the Free Software Foundation, Inc., 51 Franklin Street,
# Boston, MA 02110-1301, USA.
#

from gnuradio import gr, gr_unittest
from gnuradio import blocks
import pmt
import time

# this test tests message strobe and message debug blocks against each other
class qa_message_strobe(gr_unittest.TestCase):

    def setUp(self):
        self.tb = gr.top_block()

    def tearDown(self):
        self.tb = None

    def test_001_t(self):
        test_str = "test_msg"
        new_msg = "new_msg"
        period_ms = 7
        msg_strobe = blocks.message_strobe(pmt.intern(test_str), period_ms)
        msg_debug = blocks.message_debug()

        self.tb.msg_connect(msg_strobe, "strobe", msg_debug, "store")

        self.tb.start()

        self.assertTrue(msg_debug.num_messages() == 0) # 1st call, expect 0
        time.sleep(0.01) # floor(10/7) = 1
        self.assertTrue(msg_debug.num_messages() == 1) # 2nd call == 1
        time.sleep(0.01) # floor(20/7) = 2
        self.assertTrue(msg_debug.num_messages() == 2) # 3rd call == 2
        time.sleep(0.01) # floor(30/7) = 4 messages
        self.assertTrue(msg_debug.num_messages() == 4) # 4th call == 4
        msg_strobe.to_basic_block()._post(pmt.intern("set_msg"), pmt.intern(new_msg))
        time.sleep(0.01) # floor(40/7) = 5 messages

        self.tb.stop()
        self.tb.wait()

        # check data
        self.assertTrue(msg_debug.num_messages() == 5) # 5th call
        self.assertTrue(pmt.to_python(msg_debug.get_message(0)) == test_str, "mismatch string")
        self.assertTrue(pmt.to_python(msg_debug.get_message(4)) == new_msg, "failed to update string")

if __name__ == '__main__':
    gr_unittest.run(qa_message_strobe)
