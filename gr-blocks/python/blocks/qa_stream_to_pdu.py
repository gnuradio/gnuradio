#!/usr/bin/env python
# -*- coding: utf-8 -*-
# 
# Copyright 2017 Free Software Foundation, Inc.
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
from gnuradio import blocks
import pmt

class qa_stream_to_pdu(gr_unittest.TestCase):

    def setUp(self):
        self.tb = gr.top_block()

    def tearDown(self):
        self.tb = None

    def test_001_t(self):
        for pdu_type in [blocks.complex_t, blocks.float_t, blocks.byte_t]:
            tag_key = 'SOF'
            offset_key = 'offset'
            packet_length = 4

            # Trigger tag
            tag1_key = tag_key
            tag1_value = packet_length
            tag1_offset = 2
            tag1 = gr.tag_t()
            tag1.key = pmt.to_pmt(tag1_key)
            tag1.value = pmt.to_pmt(tag1_value)
            tag1.offset = tag1_offset
            
            # Want to find this tag in the PDU metadata dictionary
            tag2_key = 'snr'
            tag2_value = 22.83
            tag2_offset = 3
            tag2 = gr.tag_t()
            tag2.key = pmt.to_pmt(tag2_key)
            tag2.value = pmt.to_pmt(tag2_value)
            tag2.offset = tag2_offset
            
            # Want to find this tag in the PDU metadata dictionary
            tag3_key = 'freq'
            tag3_value = 17315
            tag3_offset = 4
            tag3 = gr.tag_t()
            tag3.key = pmt.to_pmt(tag3_key)
            tag3.value = pmt.to_pmt(tag3_value)
            tag3.offset = tag3_offset

            # Want this tag to get ignored
            tag4_key = tag_key
            tag4_value = packet_length
            tag4_offset = 4
            tag4 = gr.tag_t()
            tag4.key = pmt.to_pmt(tag4_key)
            tag4.value = pmt.to_pmt(tag4_value)
            tag4.offset = tag4_offset

            source_data = (0,1,2,3,4,5,6,7,8,9)
            packet_data = (2,3,4,5)


            # Test fixed-length mode
            if pdu_type == blocks.complex_t:
                source = blocks.vector_source_c(source_data, False, 1, (tag1,tag2,tag3,tag4))
            elif pdu_type == blocks.float_t:
                source = blocks.vector_source_f(source_data, False, 1, (tag1,tag2,tag3,tag4))
            else:
                source = blocks.vector_source_b(source_data, False, 1, (tag1,tag2,tag3,tag4))
            stream_to_pdu = blocks.stream_to_pdu(pdu_type, True, tag_key, offset_key, packet_length)
            sink = blocks.message_debug()

            self.tb.connect(source, stream_to_pdu)
            self.tb.msg_connect(stream_to_pdu, 'pdus', sink, 'store')
            self.tb.run()

            pdu = sink.get_message(0)
            pdu_meta = pmt.car(pdu)
            pdu_vector = pmt.cdr(pdu)

            meta_data = pmt.to_python(pdu_meta)
            vector_data = tuple(pmt.to_python(pdu_vector))

            self.assertEqual(sink.num_messages(), 1)
            self.assertAlmostEqual(vector_data, packet_data)
            self.assertEqual(meta_data, {offset_key: tag1_offset, tag2_key: tag2_value, tag3_key: tag3_value})


            # Test variable-length mode
            if pdu_type == blocks.complex_t:
                source = blocks.vector_source_c(source_data, False, 1, (tag1,tag2,tag3,tag4))
            elif pdu_type == blocks.float_t:
                source = blocks.vector_source_f(source_data, False, 1, (tag1,tag2,tag3,tag4))
            else:
                source = blocks.vector_source_b(source_data, False, 1, (tag1,tag2,tag3,tag4))
            stream_to_pdu = blocks.stream_to_pdu(pdu_type, False, tag_key, offset_key)
            sink = blocks.message_debug()

            self.tb.connect(source, stream_to_pdu)
            self.tb.msg_connect(stream_to_pdu, 'pdus', sink, 'store')
            self.tb.run()

            pdu = sink.get_message(0)
            pdu_meta = pmt.car(pdu)
            pdu_vector = pmt.cdr(pdu)

            meta_data = pmt.to_python(pdu_meta)
            vector_data = tuple(pmt.to_python(pdu_vector))

            self.assertEqual(sink.num_messages(), 1)
            self.assertAlmostEqual(vector_data, packet_data)
            self.assertEqual(meta_data, {offset_key: tag1_offset, tag2_key: tag2_value, tag3_key: tag3_value})


if __name__ == '__main__':
    gr_unittest.run(qa_stream_to_pdu, 'qa_stream_to_pdu.xml')
