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
# similar tests contained in message_debug class

# this tests the periodic message output and the input port to change the message
class qa_message_strobe(gr_unittest.TestCase):

    def setUp(self):
        self.tb = gr.top_block()

    def tearDown(self):
        self.tb = None

    def test_001_t(self):
        test_str = "test_msg"
        new_msg = "new_msg"
        period_ms = 100
        msg_strobe = blocks.message_strobe(pmt.intern(test_str), period_ms)
        msg_debug = blocks.message_debug()

        self.tb.msg_connect(msg_strobe, "strobe", msg_debug, "store")

        self.tb.start()

        self.assertEqual(msg_debug.num_messages(), 0) # 1st call, expect 0
        time.sleep(0.16) # floor(160/100) = 1
        self.assertAlmostEqual(msg_debug.num_messages(), 1, delta=1) # 2nd call == 1
        time.sleep(0.2) # floor(360/100) = 3
        self.assertAlmostEqual(msg_debug.num_messages(), 3, delta=1) # 3th call == 3
        msg_strobe.to_basic_block()._post(pmt.intern("set_msg"), pmt.intern(new_msg))
        time.sleep(0.1) # floor(460/100) = 4 messages

        self.tb.stop()
        self.tb.wait()

        # check data
        no_msgs = msg_debug.num_messages()
        self.assertAlmostEqual(no_msgs, 4, delta=1) # 4th call
        self.assertEqual(pmt.to_python(msg_debug.get_message(0)), test_str, "mismatch initial test string")
        self.assertEqual(pmt.to_python(msg_debug.get_message(no_msgs - 1)), new_msg, "failed to update string")

if __name__ == '__main__':
    gr_unittest.run(qa_message_strobe)
