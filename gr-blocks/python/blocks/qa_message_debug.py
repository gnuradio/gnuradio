#!/usr/bin/env python
#
# Copyright 2021 Malte Lenhart
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#
#

from gnuradio import gr, gr_unittest
from gnuradio import blocks
import pmt
import time

# this test tests message strobe and message debug blocks against each other
# similar tests contained in message_strobe class

# this tests only the store port and the message retrieval methods of the debug block
# print() and print_pdu() were omitted as they print to stdout


class qa_message_debug(gr_unittest.TestCase):

    def setUp(self):
        self.tb = gr.top_block()

    def tearDown(self):
        self.tb = None

    def test_001_t(self):
        test_str = "test_msg"
        new_msg = "new_msg"
        message_period_ms = 100
        msg_strobe = blocks.message_strobe(
            pmt.intern(test_str), message_period_ms)
        msg_debug = blocks.message_debug()

        self.tb.msg_connect(msg_strobe, "strobe", msg_debug, "store")

        self.tb.start()

        self.assertAlmostEqual(msg_debug.num_messages(),
                               0, delta=2)  # 1st call, expect 0
        time.sleep(1.05)  # floor(1050/100) = 10
        self.assertAlmostEqual(msg_debug.num_messages(),
                               10, delta=8)  # 2nd call == 10
        time.sleep(1)  # floor(2050/100) = 20
        self.assertAlmostEqual(msg_debug.num_messages(),
                               20, delta=10)  # 3rd call == 20

        # change test message
        msg_strobe.to_basic_block()._post(pmt.intern("set_msg"), pmt.intern(new_msg))
        time.sleep(1)

        self.tb.stop()
        self.tb.wait()
        # check data
        # first received message matches initial test message
        self.assertAlmostEqual(pmt.to_python(msg_debug.get_message(
            0)), test_str, "mismatch initial test string")

        # last message matches changed test message
        no_msgs = msg_debug.num_messages()
        self.assertAlmostEqual(pmt.to_python(msg_debug.get_message(
            no_msgs - 1)), new_msg, "failed to update string")


if __name__ == '__main__':
    gr_unittest.run(qa_message_debug)
