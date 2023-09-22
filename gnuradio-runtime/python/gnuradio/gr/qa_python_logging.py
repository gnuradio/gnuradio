#!/usr/bin/env python
#
# Copyright 2022,2023 Marcus Müller
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

        for level in (levels.debug, levels.info, levels.warn, levels.err, levels.critical):
            gr.logging().set_default_level(level)
            self.assertEqual(gr.logging().default_level(), level, f"not working for {level}")
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


class logging_hier(gr.hier_block2):
    MAGIC_STRING = "magic value"

    def __init__(self):
        gr.hier_block2.__init__(self, "python hier", gr.io_signature(0, 0, 0), gr.io_signature(0, 0, 0))
        self.logger.error(self.MAGIC_STRING)


class test_hier_logging(gr_unittest.TestCase):
    def setUp(self):
        self.blockname = "Py Hier Blk python hier"
        self.backend = gr.dictionary_logger_backend()
        gr.logging().add_default_sink(self.backend)

    def tearDown(self):
        pass

    def test_001_log(self):
        gr.logging().set_default_level(gr.log_levels.debug)
        self.dut = logging_hier()
        time.sleep(0.1)
        logged = self.backend.get_map()
        my_block_log = logged[self.blockname]
        expected = (self.dut.MAGIC_STRING,)
        for expectation, log_entry in zip(expected, sorted(my_block_log)):
            self.assertEqual(expectation, log_entry[1])


class logging_tb(gr.top_block):
    BLOCKNAME = "Aqualung"

    def __init__(self, init_msgs=None, start_msgs=None, stop_msgs=None):
        gr.top_block.__init__(self, self.BLOCKNAME)
        self.init_msgs = init_msgs or []
        self.start_msgs = start_msgs or []
        self.stop_msgs = stop_msgs or []

        for msg in self.init_msgs:
            self.logger.debug("init " + msg)

    def start(self, *args, **kwargs):
        for msg in self.start_msgs:
            self.logger.debug("start " + msg)
        gr.top_block.__init__(self, *args, **kwargs)

    def stop(self, *args, **kwargs):
        for msg in self.stop_msgs:
            self.logger.debug("stop " + msg)
        gr.top_block.__init__(self, *args, **kwargs)


class test_topblock_logging(gr_unittest.TestCase):
    def setUp(self):
        gr.logging().set_default_level(gr.log_levels.debug)
        self.backend = gr.dictionary_logger_backend()
        gr.logging().add_default_sink(self.backend)
        self.init_msgs = ["1", "2", "3"]
        self.start_msgs = ["A", "B", "C"]
        self.stop_msgs = ["D", "E", "F"]
        self.tb = logging_tb(init_msgs=self.init_msgs, start_msgs=self.start_msgs, stop_msgs=self.stop_msgs)
        self.blockname = f"Python Top Blk {self.tb.BLOCKNAME}"

    def test_001_log_init(self):
        logged = self.backend.get_map()
        print(logged)
        my_block_log = logged[self.blockname]
        expected = self.init_msgs
        for expectation, log_entry in zip(expected, sorted(my_block_log)):
            self.assertEqual("init " + expectation, log_entry[1])

    def test_002_log_start(self):
        self.tb.start()
        logged = self.backend.get_map()
        print(logged)
        my_block_log = [msg for msg in logged[self.blockname] if (not msg[1].startswith("init "))]
        expected = self.start_msgs
        for expectation, log_entry in zip(expected, sorted(my_block_log)):
            self.assertEqual("start " + expectation, log_entry[1])

    def test_003_log_stop(self):
        self.tb.stop()
        logged = self.backend.get_map()
        print(logged)
        my_block_log = [
            msg for msg in logged[self.blockname] if not (msg[1].startswith("init ") or msg[1].startswith("start "))
        ]
        expected = self.stop_msgs
        for expectation, log_entry in zip(expected, sorted(my_block_log)):
            self.assertEqual("stop " + expectation, log_entry[1])


if __name__ == "__main__":
    gr_unittest.run(test_logging)
