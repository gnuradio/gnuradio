#!/usr/bin/env python
#
# Copyright 2019 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#

import time
from gnuradio import gr, gr_unittest, iio, blocks

import pmt


class test_iio(gr_unittest.TestCase):

    def test_import(self):
        """Just see if we can import the module...
        They may not have a IIO device connected, etc.  Don't try to run anything"""
        pass

    def test_attribute_updater(self):
        attr = "Test"
        val = "100"
        key0 = pmt.intern(attr)
        val0 = pmt.intern(val)
        msg_dic = pmt.make_dict()
        msg_dic = pmt.dict_add(msg_dic, key0, val0)

        src = iio.attr_updater(attr, val, 500)
        snk = blocks.message_debug()

        tb = gr.top_block()
        tb.msg_connect((src, "out"), (snk, "store"))
        tb.start()
        time.sleep(1)
        tb.stop()
        tb.wait()

        rec_msg = snk.get_message(0)
        self.assertTrue(pmt.equal(rec_msg, msg_dic))

    def test_event_monitor(self):
        """event_monitor only exists on libiio v1 builds, and needs a
        reachable IIO context with an event-capable device. Skip otherwise
        since neither is guaranteed to be available in this environment."""
        if not hasattr(iio, 'event_monitor'):
            self.skipTest("event_monitor not available (requires libiio v1)")
            return

        try:
            src = iio.event_monitor("local:", "iio_dummy")
        except RuntimeError as e:
            self.skipTest("No IIO event-capable context available: %s" % e)
            return

        snk = blocks.message_debug()

        tb = gr.top_block()
        tb.msg_connect((src, "event"), (snk, "store"))
        tb.start()
        time.sleep(0.5)
        tb.stop()
        tb.wait()


if __name__ == '__main__':
    gr_unittest.run(test_iio)
