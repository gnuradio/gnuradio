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


from gnuradio import gr, gr_unittest, blocks


class test_logger (gr_unittest.TestCase):

    def setUp(self):
        pass

    def tearDown(self):
        pass

    def set_and_assert_log_level(self, block, level, ref=None):
        if ref is None:
            ref = level
        block.set_log_level(level)
        self.assertEqual(block.log_level(), ref)

    def test_log_level_for_block(self):
        # Test the python API for getting and setting log levels of individual block
        # go through all of the documented log_levels
        ns = blocks.null_source(1)
        self.set_and_assert_log_level(ns, "debug")
        self.set_and_assert_log_level(ns, "info")
        self.set_and_assert_log_level(ns, "notice", "info")
        self.set_and_assert_log_level(ns, "warn", "warning")
        self.set_and_assert_log_level(ns, "warning")
        self.set_and_assert_log_level(ns, "error")
        self.set_and_assert_log_level(ns, "crit", "critical")
        self.set_and_assert_log_level(ns, "critical")
        self.set_and_assert_log_level(ns, "alert", "critical")
        self.set_and_assert_log_level(ns, "emerg", "critical")
        # There's a couple of special cases. "off" == "notset" (specific to gr)
        # and "fatal" == "emerg" (built in to log4cpp)
        ns.set_log_level("off")
        self.assertEqual(ns.log_level(), "off")

    def test_log_level_for_tb(self):
        # Test the python API for getting and setting log levels for a
        # top_block
        nsrc = blocks.null_source(4)
        nsnk = blocks.null_sink(4)
        # Set all log levels to a known state
        nsrc.set_log_level("debug")
        nsnk.set_log_level("debug")
        tb = gr.top_block()
        tb.connect(nsrc, nsnk)
        # confirm that the tb has log_level of first block
        self.assertEqual(tb.log_level(), "debug")
        # confirm that changing tb log_level propagates to connected blocks
        tb.set_log_level("critical")
        self.assertEqual(tb.log_level(), "critical")
        self.assertEqual(nsrc.log_level(), "critical")
        self.assertEqual(nsnk.log_level(), "critical")

    def test_log_level_for_hier_block(self):
        # Test the python API for getting and setting log levels for hier
        # blocks
        nsrc = blocks.null_source(4)
        nsnk = blocks.null_sink(4)
        b = blocks.stream_to_vector_decimator(
            4, 1, 1, 1)  # just a random hier block that exists
        tb = gr.top_block()
        tb.connect(nsrc, b, nsnk)
        tb.set_log_level("debug")
        self.assertEqual(tb.log_level(), "debug")
        self.assertEqual(nsrc.log_level(), "debug")
        self.assertEqual(nsnk.log_level(), "debug")
        self.assertEqual(b.one_in_n.log_level(), "debug")
        tb.set_log_level("critical")
        self.assertEqual(tb.log_level(), "critical")
        self.assertEqual(nsrc.log_level(), "critical")
        self.assertEqual(nsnk.log_level(), "critical")
        self.assertEqual(b.one_in_n.log_level(), "critical")


if __name__ == '__main__':
    gr_unittest.run(test_logger)
