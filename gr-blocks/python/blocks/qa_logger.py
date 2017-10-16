#!/usr/bin/env python
#
# Copyright 2016 Free Software Foundation, Inc.
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


from gnuradio import gr, gr_unittest, blocks

class test_logger (gr_unittest.TestCase):

    def setUp(self):
        pass

    def tearDown(self):
        pass

    def set_and_assert_log_level(self, block, level):
        block.set_log_level(level)
        self.assertEqual(block.log_level(), level)

    def test_log_level_for_block(self):
        # Test the python API for getting and setting log levels of individual block
        # go through all of the documented log_levels
        ns = blocks.null_source(1)
        self.set_and_assert_log_level(ns, "notset")
        self.set_and_assert_log_level(ns, "debug")
        self.set_and_assert_log_level(ns, "info")
        self.set_and_assert_log_level(ns, "notice")
        self.set_and_assert_log_level(ns, "warn")
        self.set_and_assert_log_level(ns, "error")
        self.set_and_assert_log_level(ns, "crit")
        self.set_and_assert_log_level(ns, "alert")
        self.set_and_assert_log_level(ns, "emerg")
        # There's a couple of special cases. "off" == "notset" (specific to gr)
        # and "fatal" == "emerg" (built in to log4cpp)
        ns.set_log_level("off")
        self.assertEqual(ns.log_level(), "notset")
        ns.set_log_level("fatal")
        self.assertEqual(ns.log_level(), "emerg")
        # Make sure exception is throw on bogus data
        self.assertRaises(RuntimeError, ns.set_log_level, "11")


    def test_log_level_for_tb(self):
        # Test the python API for getting and setting log levels for a top_block
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
        tb.set_log_level("alert")
        self.assertEqual(tb.log_level(), "alert")
        self.assertEqual(nsrc.log_level(), "alert")
        self.assertEqual(nsnk.log_level(), "alert")

    def test_log_level_for_hier_block(self):
        # Test the python API for getting and setting log levels for hier blocks
        nsrc = blocks.null_source(4)
        nsnk = blocks.null_sink(4)
        b = blocks.stream_to_vector_decimator(4, 1, 1, 1) # just a random hier block that exists
        tb = gr.top_block()
        tb.connect(nsrc, b, nsnk)
        tb.set_log_level("debug")
        self.assertEqual(tb.log_level(), "debug")
        self.assertEqual(nsrc.log_level(), "debug")
        self.assertEqual(nsnk.log_level(), "debug")
        self.assertEqual(b.one_in_n.log_level(), "debug")
        tb.set_log_level("alert")
        self.assertEqual(tb.log_level(), "alert")
        self.assertEqual(nsrc.log_level(), "alert")
        self.assertEqual(nsnk.log_level(), "alert")
        self.assertEqual(b.one_in_n.log_level(), "alert")

if __name__ == '__main__':
    gr_unittest.run(test_logger, "test_logger.xml")
