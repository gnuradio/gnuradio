#!/usr/bin/env python
# -*- coding: utf-8 -*-
#
# Copyright 2004,2010,2013,2016 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#
#

import time

from gnuradio import gr, gr_unittest, blocks
import pmt


# def all_counts():
#     return (gr.block_ncurrently_allocated(),
#             gr.block_detail_ncurrently_allocated(),
#             gr.buffer_ncurrently_allocated(),
#             gr.buffer_reader_ncurrently_allocated(),
#             gr.message_ncurrently_allocated())


class test_message(gr_unittest.TestCase):

    # def setUp(self):
    #     # self.msgq = gr.msg_queue()

    # def tearDown(self):
    #     # self.msgq = None

    # def leak_check(self, fct):
    #     begin = all_counts()
    #     fct()
    #     # tear down early so we can check for leaks
    #     self.tearDown()
    #     end = all_counts()
    #     self.assertEqual(begin, end)

    def test_100(self):
        msg = gr.message(0, 1.5, 2.3)
        self.assertEqual(0, msg.type())
        self.assertAlmostEqual(1.5, msg.arg1())
        self.assertAlmostEqual(2.3, msg.arg2())
        self.assertEqual(0, msg.length())

    def test_101(self):
        s = b'This is a test'
        msg = gr.message_from_string(s.decode('utf8'))
        self.assertEqual(s, msg.to_string())

    def test_102_unicodechars(self):
        s = u"(╯°□°)╯︵ ┻━┻"
        msg = gr.message_from_string(s)
        self.assertEqual(s.encode('utf8'), msg.to_string())

    # msg_queue was removed from API in 3.8
    # def test_200(self):
    #     self.leak_check(self.body_200)

    # def body_200(self):
    #     self.msgq.insert_tail(gr.message(0))
    #     self.assertEqual(1, self.msgq.count())
    #     self.msgq.insert_tail(gr.message(1))
    #     self.assertEqual(2, self.msgq.count())
    #     msg0 = self.msgq.delete_head()
    #     self.assertEqual(0, msg0.type())
    #     msg1 = self.msgq.delete_head()
    #     self.assertEqual(1, msg1.type())
    #     self.assertEqual(0, self.msgq.count())

    # def test_201(self):
    #     self.leak_check(self.body_201)

    # def body_201(self):
    #     self.msgq.insert_tail(gr.message(0))
    #     self.assertEqual(1, self.msgq.count())
    #     self.msgq.insert_tail(gr.message(1))
    #     self.assertEqual(2, self.msgq.count())

    # def test_202(self):
    #     self.leak_check(self.body_202)

    def body_202(self):
        # global msg
        msg = gr.message(666)

    def test_300(self):
        input_data = [0, 1, 2, 3, 4, 5, 6, 7, 8, 9]
        src = blocks.vector_source_b(input_data)
        dst = blocks.vector_sink_b()
        tb = gr.top_block()
        tb.connect(src, dst)
        tb.run()
        self.assertEqual(input_data, dst.data())

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
    gr_unittest.run(test_message)
