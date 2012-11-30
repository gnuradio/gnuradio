#!/usr/bin/env python
#
# Copyright 2012 Free Software Foundation, Inc.
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

from gnuradio import gr, gr_unittest
from gruel import pmt
import time;
class test_pdu(gr_unittest.TestCase):

    def setUp(self):
        self.tb = gr.top_block()

    def tearDown(self):
        self.tb = None

    def test_000(self):
        # Just run some data through and make sure it doesn't puke.
        src_data = (1, 2, 3, 4, 5, 6, 7, 8, 9, 10)

        src = gr.pdu_to_tagged_stream(gr.BYTE);
        snk3 = gr.tagged_stream_to_pdu(gr.BYTE);
        snk2 = gr.vector_sink_b();
        snk = gr.tag_debug(1, "test");

        dbg = gr.message_debug();

        self.tb.connect(src, snk)
        self.tb.connect(src, snk2)
        self.tb.connect(src, snk3)

        self.tb.msg_connect(snk3, "pdus", dbg, "print");
        self.tb.start()

        # make our reference and message pmts
        port = pmt.pmt_intern("pdus");
        msg = pmt.pmt_cons( pmt.PMT_NIL, pmt.pmt_make_u8vector(16, 0xFF) );
        
        print "printing port & msg"
        pmt.pmt_print(port);
        pmt.pmt_print(msg);

        # post the message
        src.to_basic_block()._post( port, msg );

        time.sleep(1);
        self.tb.stop();
        self.tb.wait();

        print snk2.data();

if __name__ == '__main__':
    gr_unittest.run(test_pdu, "test_pdu.xml")
