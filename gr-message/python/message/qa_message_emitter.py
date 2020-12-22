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

class qa_message_emitter (gr_unittest.TestCase):

    def setUp (self):
        self.tb = gr.top_block ()

    def tearDown (self):
        self.tb = None

    def test_001_emit3 (self):
        emitter = message.message_emitter()
        debug = blocks.message_debug()
        self.tb.msg_connect((emitter, 'msg'), (debug, 'store'))

        self.tb.start()
        time.sleep(.001)
        emitter.emit()
        time.sleep(.001)
        emitter.emit()
        time.sleep(.001)
        emitter.emit()
        time.sleep(.01)
        self.tb.stop()
        self.tb.wait()
        
        self.assertEquals(3, debug.num_messages())
        self.assertEquals(3, emitter.get_n_msgs() )

    def test_002_emit2 (self):
        emitter = message.message_emitter()
        debug = blocks.message_debug()
        msg = pmt.cons(pmt.intern("TEST MESSAGE"), pmt.init_u8vector(4,range(4)))
        self.tb.msg_connect((emitter, 'msg'), (debug, 'store'))

        self.tb.start()
        time.sleep(.001)
        emitter.emit()
        time.sleep(.001)
        emitter.emit(msg)
        time.sleep(.01)
        self.tb.stop()
        self.tb.wait()

        print(emitter.get_n_msgs())
        self.assertEquals(2, debug.num_messages())
        self.assertEquals(2, emitter.get_n_msgs() )
        self.assertTrue(pmt.eqv(pmt.PMT_NIL, debug.get_message(0)))
        self.assertTrue(pmt.eqv(msg, debug.get_message(1)))


if __name__ == '__main__':
    gr_unittest.run(qa_message_emitter)
