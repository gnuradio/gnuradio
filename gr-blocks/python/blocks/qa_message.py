#!/usr/bin/env python
#
# Copyright 2004,2010,2013 Free Software Foundation, Inc.
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

import time

from gnuradio import gr, gr_unittest, blocks
import pmt

def all_counts():
    return (gr.block_ncurrently_allocated(),
            gr.block_detail_ncurrently_allocated(),
            gr.buffer_ncurrently_allocated(),
            gr.buffer_reader_ncurrently_allocated(),
            gr.message_ncurrently_allocated())


class test_message(gr_unittest.TestCase):

    def setUp(self):
        self.msgq = gr.msg_queue()

    def tearDown(self):
        self.msgq = None

    def leak_check(self, fct):
        begin = all_counts()
        fct()
        # tear down early so we can check for leaks
        self.tearDown()
        end = all_counts()
        self.assertEqual(begin, end)

    def test_100(self):
        msg = gr.message(0, 1.5, 2.3)
        self.assertEquals(0, msg.type())
        self.assertAlmostEqual(1.5, msg.arg1())
        self.assertAlmostEqual(2.3, msg.arg2())
        self.assertEquals(0, msg.length())

    def test_101(self):
        s = 'This is a test'
        msg = gr.message_from_string(s)
        self.assertEquals(s, msg.to_string())

    def test_200(self):
        self.leak_check(self.body_200)

    def body_200(self):
        self.msgq.insert_tail(gr.message(0))
        self.assertEquals(1, self.msgq.count())
        self.msgq.insert_tail(gr.message(1))
        self.assertEquals(2, self.msgq.count())
        msg0 = self.msgq.delete_head()
        self.assertEquals(0, msg0.type())
        msg1 = self.msgq.delete_head()
        self.assertEquals(1, msg1.type())
        self.assertEquals(0, self.msgq.count())

    def test_201(self):
        self.leak_check(self.body_201)

    def body_201(self):
        self.msgq.insert_tail(gr.message(0))
        self.assertEquals(1, self.msgq.count())
        self.msgq.insert_tail(gr.message(1))
        self.assertEquals(2, self.msgq.count())

    def test_202(self):
        self.leak_check(self.body_202)

    def body_202(self):
        # global msg
        msg = gr.message(666)

    def test_300(self):
        input_data = (0,1,2,3,4,5,6,7,8,9)
        src = blocks.vector_source_b(input_data)
        dst = blocks.vector_sink_b()
	tb = gr.top_block()
        tb.connect(src, dst)
        tb.run()
        self.assertEquals(input_data, dst.data())

    def test_301(self):
        # Use itemsize, limit constructor
        src = blocks.message_source(gr.sizeof_char)
        dst = blocks.vector_sink_b()
	tb = gr.top_block()
        tb.connect(src, dst)
        src.msgq().insert_tail(gr.message_from_string('01234'))
        src.msgq().insert_tail(gr.message_from_string('5'))
        src.msgq().insert_tail(gr.message_from_string(''))
        src.msgq().insert_tail(gr.message_from_string('6789'))
        src.msgq().insert_tail(gr.message(1))                  # send EOF
        tb.run()
        self.assertEquals(tuple(map(ord, '0123456789')), dst.data())

    def test_302(self):
        # Use itemsize, msgq constructor
        msgq = gr.msg_queue()
        src = blocks.message_source(gr.sizeof_char, msgq)
        dst = blocks.vector_sink_b()
	tb = gr.top_block()
        tb.connect(src, dst)
        src.msgq().insert_tail(gr.message_from_string('01234'))
        src.msgq().insert_tail(gr.message_from_string('5'))
        src.msgq().insert_tail(gr.message_from_string(''))
        src.msgq().insert_tail(gr.message_from_string('6789'))
        src.msgq().insert_tail(gr.message(1))                  # send EOF
        tb.run()
        self.assertEquals(tuple(map(ord, '0123456789')), dst.data())

    def test_debug_401(self):
        msg = pmt.intern("TESTING")
        src = blocks.message_strobe(msg, 500)
        snk = blocks.message_debug()

	tb = gr.top_block()
        tb.msg_connect(src, "strobe", snk, "store")
        tb.start()
        time.sleep(1)
        tb.stop()
        tb.wait()

        rec_msg = snk.get_message(0)
        self.assertTrue(pmt.eqv(rec_msg, msg))


if __name__ == '__main__':
    gr_unittest.run(test_message, "test_message.xml")
