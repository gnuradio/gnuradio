#!/usr/bin/env python
#
# Copyright 2023 Marcus Müller
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#
#


from gnuradio import blocks
from gnuradio import gr_unittest
from gnuradio import gr
import pmt
import sys
import time


class test_msg_pair_to_var(gr_unittest.TestCase):

    def setUp(self):
        self.backend = gr.dictionary_logger_backend()
        gr.logging().add_default_sink(self.backend)
        self.tb = gr.top_block(catch_exceptions=False)

    def tearDown(self):
        self.tb = None

    def test_calling(self):
        canary = 0xDEAFBEEF
        self.test_val = 0

        def test_f(value: int = 0):
            self.test_val = value

        dut = blocks.msg_pair_to_var(test_f)
        test_msg = pmt.cons(pmt.intern("foo"), pmt.from_uint64(canary))
        src = blocks.message_strobe(test_msg, 10)
        self.tb.msg_connect(*(src, pmt.intern("strobe")),
                            *(dut, blocks.msg_pair_to_var.IN_PORT))
        self.tb.start()
        time.sleep(15e-3)
        self.tb.stop()
        self.tb.wait()

        self.assertEqual(self.test_val, canary,
                         f"Test value {hex(self.test_val)} not found to be canary {hex(canary)}")

    def test_exceptions(self):
        lark = 0xDEAFBEEF
        nightingale = 0xDEAFB16D
        self.test_val = 0

        def test_f(value: int = 0):
            if value == nightingale and value != lark:
                raise Exception("It was the nightingale, and not the lark")

        dut = blocks.msg_pair_to_var(test_f)
        test_msg = pmt.cons(pmt.intern("birb"), pmt.from_uint64(nightingale))
        src = blocks.message_strobe(test_msg, 10)
        self.tb.msg_connect(*(src, pmt.intern("strobe")),
                            *(dut, blocks.msg_pair_to_var.IN_PORT))
        self.tb.start()
        time.sleep(50e-3)
        self.tb.stop()
        self.tb.wait()

        logged = self.backend.get_map()
        self.assertIn("msg_pair_to_var", logged, "should have logged; didn't.")
        log_entries = logged["msg_pair_to_var"]
        for timestamp, entry in log_entries:
            self.assertRegex(
                entry,
                r"Error when calling <function .*\.test_exceptions\..*.test_f.*> with 3736056173" +
                r".*reason: It was the nightingale, and not the lark.*",
                f"Log entry '{entry}' from {timestamp} doesn't contain Shakespeare")


if __name__ == '__main__':
    gr_unittest.run(test_msg_pair_to_var)
