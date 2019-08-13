#!/usr/bin/env python
#
# Copyright 2019 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#

from __future__ import print_function
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

        src = iio.attr_updater(attr,val,500)
        snk = blocks.message_debug()

        tb = gr.top_block()
        tb.msg_connect((src, "out"), (snk, "store"))
        tb.start()
        time.sleep(1)
        tb.stop()
        tb.wait()

        rec_msg = snk.get_message(0)
        self.assertTrue(pmt.equal(rec_msg, msg_dic))

if __name__ == '__main__':
    gr_unittest.run(test_iio, "test_iio.xml")
