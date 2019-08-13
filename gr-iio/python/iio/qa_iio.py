#!/usr/bin/env python
#
# Copyright 2019 Free Software Foundation, Inc.
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
