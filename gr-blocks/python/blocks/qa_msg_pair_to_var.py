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
        src = blocks.message_strobe(test_msg, 50)
        self.tb.msg_connect(*(src, pmt.intern("strobe")),
                            *(dut, blocks.msg_pair_to_var.IN_PORT))

        start_time = time.monotonic_ns()
        self.tb.start()

        # Sleep for at most a second
        for counter in range(20):
            time.sleep(50e-3)
            if self.test_val == canary:
                break
        self.tb.stop()
        self.tb.wait()
        stopped_time = time.monotonic_ns()
        timecount = round((stopped_time - start_time) * 1e-6)

        self.assertEqual(
            self.test_val,
            canary,
            f"Test value {hex(self.test_val)} not found to be canary {hex(canary)} within {timecount} ms",
        )

    def test_exceptions(self):
        lark = 0xDEAFBEEF
        nightingale = 0xDEAFB16D
        self.test_val = False

        def test_f(value: int = 0):
            if value == nightingale and value != lark:
                self.test_val = True
                raise Exception("It was the nightingale, and not the lark")

        dut = blocks.msg_pair_to_var(test_f)
        test_msg = pmt.cons(pmt.intern("birb"), pmt.from_uint64(nightingale))
        src = blocks.message_strobe(test_msg, 50)
        self.tb.msg_connect(
            *(src, pmt.intern("strobe")), *(dut, blocks.msg_pair_to_var.IN_PORT)
        )

        start_time = time.monotonic_ns()
        self.tb.start()

        # Sleep for at most a second
        for counter in range(19):
            time.sleep(50e-3)
            if self.test_val:
                break
        # some time for Exception bubbling, which should happen without yields
        time.sleep(50e-3)
        self.tb.stop()
        self.tb.wait()
        stopped_time = time.monotonic_ns()
        timecount = round((stopped_time - start_time) * 1e-6)

        logged = self.backend.get_map()
        self.assertIn("msg_pair_to_var", logged, f"should have logged; didn't within {timecount} ms.")
        log_entries = logged["msg_pair_to_var"]
        for timestamp, entry in log_entries:
            self.assertRegex(
                entry,
                r"Error when calling <function .*\.test_exceptions\..*.test_f.*> with 3736056173" +
                r".*reason: It was the nightingale, and not the lark.*",
                f"Log entry '{entry}' from {timestamp} doesn't contain Shakespeare",
            )


if __name__ == "__main__":
    gr_unittest.run(test_msg_pair_to_var)
