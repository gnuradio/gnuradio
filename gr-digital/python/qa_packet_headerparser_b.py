#!/usr/bin/env python
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

import time
from gnuradio import gr, gr_unittest
try: import pmt
except: from gruel import pmt
import blocks_swig as blocks
import digital_swig as digital

class qa_packet_headerparser_b (gr_unittest.TestCase):

    def setUp (self):
        self.tb = gr.top_block ()

    def tearDown (self):
        self.tb = None

    def test_001_t (self):
        expected_data = (
            #   | Number of symbols                  | Packet number                                  | Parity
                0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  1, 0, 0, 0,
                0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0,
                0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0,  0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  1, 0, 0, 0
        )
        tagname = "packet_len"

        src = gr.vector_source_b(expected_data)
        parser = digital.packet_headerparser_b(32, tagname)
        sink = blocks.message_debug()

        self.tb.connect(src, parser)
        self.tb.msg_connect(parser, "header_data", sink, "store")
        self.tb.start ()
        time.sleep(1)
        self.tb.stop()
        self.tb.wait()

        self.assertEqual(sink.num_messages(), 3)
        msg = sink.get_message(0)
        #try:
        #self.assertEqual(4, pmt.pmt_to_long(pmt.pmt_dict_ref(msg, pmt.pmt_string_to_symbol(tagname), pmt.PMT_F)))
        #self.assertEqual(0, pmt.pmt_to_long(pmt.pmt_dict_ref(msg, pmt.pmt_string_to_symbol("packet_num"), pmt.PMT_F)))

        #except:
            #self.fail()
        # msg1: length 4, number 0
        # msg2: length 2, number 1
        # msg3: PMT_F because parity fail



if __name__ == '__main__':
    gr_unittest.run(qa_packet_headerparser_b, "qa_packet_headerparser_b.xml")
