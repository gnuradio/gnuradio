#!/usr/bin/env python
#
# Copyright 2022 Marcus Müller
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#
#

from gnuradio import gr, gr_unittest
import numpy
import time


class logging_blk(gr.sync_block):
    def __init__(self, name=""):
        gr.sync_block.__init__(self, name, in_sig=[], out_sig=[numpy.float32])

        self.logger.info("Initialized")

    def start(self):
        self.logger.info("Started")

    def stop(self):
        self.logger.info("Stopped")

    def work(self, _, out_streams):
        if self.nitems_written(0) < 10**6:
            return len(out_streams[0])
        return -1


class drop_sink(gr.sync_block):
    def __init__(self, name=""):
        gr.sync_block.__init__(self, name, in_sig=[numpy.float32], out_sig=[])

    def work(self, in_streams, _):
        return len(in_streams[0])


class test_logging(gr_unittest.TestCase):
    def setUp(self):
        self.blockname = "logger test"
        self.backend = gr.dictionary_logger_backend()
        gr.logging().add_default_sink(self.backend)

        self.tb = gr.top_block()
        self.blk = logging_blk(self.blockname)
        self.sink = drop_sink()

        self.tb.connect(self.blk, self.sink)

    def tearDown(self):
        self.tb = None

    def test_000_level(self):
        levels = gr.log_levels

        for level in (levels.debug, levels.info, levels.warn, levels.err,
                      levels.critical):
            gr.logging().set_default_level(level)
            self.assertEqual(gr.logging().default_level(), level,
                             f"not working for {level}")
        gr.logging().set_default_level(levels.info)

    def test_001_log(self):
        gr.logging().set_default_level(gr.log_levels.debug)
        self.tb.run(max_noutput_items=1024)
        time.sleep(0.1)
        logged = self.backend.get_map()
        my_block_log = logged[self.blockname]
        expected = ("Initialized", "Started", "Stopped")
        for expectation, log_entry in zip(expected, sorted(my_block_log)):
            self.assertEqual(expectation, log_entry[1])


class test_disabled_logging(gr_unittest.TestCase):
    def setUp(self):
        self.blockname = "logger test"
        gr.logging().set_default_level(gr.log_levels.off)
        self.backend = gr.dictionary_logger_backend()
        gr.logging().add_default_sink(self.backend)

        self.tb = gr.top_block()
        self.blk = logging_blk(self.blockname)
        self.sink = drop_sink()

        self.tb.connect(self.blk, self.sink)

    def test_001_log_disabled(self):
        self.tb.run(max_noutput_items=1024)
        time.sleep(0.1)
        logged = self.backend.get_map()
        self.assertNotIn(self.blockname, logged)


if __name__ == '__main__':
    gr_unittest.run(test_logging)
