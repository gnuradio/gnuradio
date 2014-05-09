#!/usr/bin/env python
#
# Copyright 2006,2007,2010,2011,2013 Free Software Foundation, Inc.
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

from gnuradio import gr, gr_unittest, digital, blocks
import pmt
import numpy
from time import sleep

class test_hdlc_framer(gr_unittest.TestCase):
    def setUp(self):
        self.tb = gr.top_block()

    def tearDown(self):
        self.tb = None

    def test_001(self):
        #test complementary operation of framer & deframer
        #want to frame some random data that has enough consecutive bits to
        #stuff at least a few bits
        npkts = 20
        src_data = [0xFE, 0xDA, 0xAC, 0x29, 0x7F, 0xA2, 0x90, 0x0F, 0xF8]
        frame   = digital.hdlc_framer_pb("wat")
        deframe = digital.hdlc_deframer_bp(8, 500)
        debug   = blocks.message_debug()
        self.tb.connect(frame, deframe)
        self.tb.msg_connect(deframe, "out", debug, "store")
        self.tb.start()
        msg = pmt.cons(pmt.PMT_NIL, pmt.init_u8vector(len(src_data),src_data))
        for i in xrange(npkts):
            frame.to_basic_block()._post(pmt.intern("in"), msg)
        sleep(0.2)
        self.tb.stop()
        self.tb.wait()
        rxmsg = debug.get_message(0)
        result_len = pmt.blob_length(pmt.cdr(rxmsg))
        msg_data = []
        for j in xrange(result_len):
            msg_data.append(pmt.u8vector_ref(pmt.cdr(rxmsg), j))
        self.assertEqual(src_data, msg_data)


if __name__ == '__main__':
    gr_unittest.run(test_hdlc_framer, "test_hdlc_framer.xml")

