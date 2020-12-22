#!/usr/bin/env python
#
# Copyright 2020 NTESS LLC
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#


from gnuradio import gr, gr_unittest
from gnuradio import blocks, message
import pmt
import time

class qa_message_counter (gr_unittest.TestCase):

    def setUp(self):
        self.tb = gr.top_block()
        self.emitter = message.message_emitter()
        self.ctr = message.message_counter("counter")
        self.tb.msg_connect((self.emitter, 'msg'), (self.ctr, 'msg'))

    def tearDown(self):
        self.tb = None

    def test_001_5x_pass(self):

        self.tb.start()
        for x in range(5):
            time.sleep(.001)
            self.emitter.emit()
        time.sleep(.01)
        self.tb.stop()
        self.tb.wait()

        self.assertEqual(5, self.ctr.get_ctr())

    def test_002_rst_3x_pass(self):

        self.tb.start()
        for x in range(5):
            time.sleep(.001)
            self.emitter.emit()
        time.sleep(.1)
        self.assertEqual(5, self.ctr.get_ctr())

        self.ctr.reset()
        self.assertEqual(0, self.ctr.get_ctr())

        for x in range(3):
            time.sleep(.001)
            self.emitter.emit()
        time.sleep(.1)

        self.tb.stop()
        self.tb.wait()

        self.assertEqual(3, self.ctr.get_ctr())

    def test_003_get_name(self):
        '''
        tests get_name function
        '''

        print('test_003_get_name() - name is ', self.ctr.get_name())
        self.assertEqual('counter', self.ctr.get_name())

        self.ctr = message.message_counter("bubba")
        self.assertEqual('bubba', self.ctr.get_name())


if __name__ == '__main__':
    gr_unittest.run(qa_message_counter)
