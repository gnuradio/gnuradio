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
import random

from gnuradio import gr, gr_unittest, blocks, digital
from gnuradio.digital.utils import tagged_streams
import pmt

class qa_packet_headerparser_b (gr_unittest.TestCase):

    def setUp (self):
        self.tb = gr.top_block ()

    def tearDown (self):
        self.tb = None

    def test_001_t (self):
        """
        First header: Packet length 4, packet num 0
        Second header: Packet 2, packet num 1
        Third header: Invalid (CRC does not check) (would be len 4, num 2)
        """
        encoded_headers = (
            #   | Number of bytes                    | Packet number                      | CRC
                0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  1, 0, 0, 1, 0, 0, 0, 1,
                0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 1, 0, 1, 1, 1,
                0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0,  0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  1, 1, 1, 1, 1, 1, 1, 1
        )
        packet_len_tagname = "packet_len"
        random_tag = gr.tag_t()
        random_tag.offset = 5
        random_tag.key = pmt.string_to_symbol("foo")
        random_tag.value = pmt.from_long(42)
        src = blocks.vector_source_b(encoded_headers, tags=(random_tag,))
        parser = digital.packet_headerparser_b(32, packet_len_tagname)
        sink = blocks.message_debug()
        self.tb.connect(src, parser)
        self.tb.msg_connect(parser, "header_data", sink, "store")
        self.tb.start()
        time.sleep(1)
        self.tb.stop()
        self.tb.wait()
        self.assertEqual(sink.num_messages(), 3)
        msg1 = pmt.to_python(sink.get_message(0))
        msg2 = pmt.to_python(sink.get_message(1))
        msg3 = pmt.to_python(sink.get_message(2))
        self.assertEqual(msg1, {'packet_len': 4, 'packet_num': 0, 'foo': 42})
        self.assertEqual(msg2, {'packet_len': 2, 'packet_num': 1})
        self.assertEqual(msg3, False)

    def test_002_pipe(self):
        """
        Create N packets of random length, pipe them through header generator,
        back to header parser, make sure output is the same.
        """
        N = 20
        header_len = 32
        packet_len_tagname = "packet_len"
        packet_lengths = [random.randint(1, 100) for x in range(N)]
        data, tags = tagged_streams.packets_to_vectors([range(packet_lengths[i]) for i in range(N)], packet_len_tagname)
        src = blocks.vector_source_b(data, False, 1, tags)
        header_gen = digital.packet_headergenerator_bb(header_len, packet_len_tagname)
        header_parser = digital.packet_headerparser_b(header_len, packet_len_tagname)
        sink = blocks.message_debug()
        self.tb.connect(src, header_gen, header_parser)
        self.tb.msg_connect(header_parser, "header_data", sink, "store")
        self.tb.start()
        time.sleep(1)
        self.tb.stop()
        self.tb.wait()
        self.assertEqual(sink.num_messages(), N)
        for i in xrange(N):
            msg = pmt.to_python(sink.get_message(i))
            self.assertEqual(msg, {'packet_len': packet_lengths[i], 'packet_num': i})

    def test_003_ofdm (self):
        """ Header 1: 193 bytes
        Header 2: 8 bytes
        2 bits per complex symbol, 32 carriers => 64 bits = 8 bytes per OFDM symbol
                                    4 carriers =>  8 bits = 1 byte  per OFDM symbol
                                    8 carriers => 16 bits = 2 bytes per OFDM symbol
        Means we need 52 carriers to store the 193 bytes.
        """
        encoded_headers = (
            #   | Number of bytes                    | Packet number                      | CRC
                1, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  1, 0, 1, 1, 0, 1, 1, 0,
                0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0,  1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  0, 0, 1, 0, 1, 1, 1, 0,
        )
        packet_len_tagname = "packet_len"
        frame_len_tagname = "frame_len"
        src = blocks.vector_source_b(encoded_headers)
        header_formatter = digital.packet_header_ofdm(
                (range(32),range(4),range(8)), # 32/4/8 carriers are occupied (which doesn't matter here)
                1,         # 1 OFDM symbol per header (= 32 bits)
                packet_len_tagname,
                frame_len_tagname,
                "packet_num",
                1,         # 1 bit per header symbols (BPSK)
                2          # 2 bits per payload symbol (QPSK)
        )
        parser = digital.packet_headerparser_b(header_formatter.base())
        sink = blocks.message_debug()
        self.tb.connect(src, parser)
        self.tb.msg_connect(parser, "header_data", sink, "store")
        self.tb.start()
        time.sleep(1)
        self.tb.stop()
        self.tb.wait()
        self.assertEqual(sink.num_messages(), 2)
        msg1 = pmt.to_python(sink.get_message(0))
        msg2 = pmt.to_python(sink.get_message(1))
        # Multiply with 4 because unpacked bytes have only two bits
        self.assertEqual(msg1, {'packet_len': 193*4, 'frame_len': 52, 'packet_num': 0})
        self.assertEqual(msg2, {'packet_len': 8*4, 'frame_len': 1, 'packet_num': 1})

    def test_004_ofdm_scramble(self):
        """
        Test scrambling for OFDM header gen
        """
        header_len = 32
        packet_length = 23
        packet_len_tagname = "packet_len"
        frame_len_tagname = "frame_len"
        data, tags = tagged_streams.packets_to_vectors([range(packet_length),range(packet_length),], packet_len_tagname)
        src = blocks.vector_source_b(data, False, 1, tags)
        header_formatter = digital.packet_header_ofdm(
                (range(32),), # 32 carriers are occupied (which doesn't matter here)
                1,         # 1 OFDM symbol per header (= 32 bits)
                packet_len_tagname,
                frame_len_tagname,
                "packet_num",
                1,         # 1 bit per header symbols (BPSK)
                2,          # 2 bits per payload symbol (QPSK)
                scramble_header=True
        )
        header_gen = digital.packet_headergenerator_bb(header_formatter.base())
        header_parser = digital.packet_headerparser_b(header_formatter.base())
        sink = blocks.message_debug()
        self.tb.connect(src, header_gen, header_parser)
        self.tb.msg_connect(header_parser, "header_data", sink, "store")
        self.tb.start()
        time.sleep(1)
        self.tb.stop()
        self.tb.wait()
        msg = pmt.to_python(sink.get_message(0))
        self.assertEqual(msg, {'packet_len': packet_length, 'packet_num': 0, 'frame_len': 4})
        msg = pmt.to_python(sink.get_message(1))
        self.assertEqual(msg, {'packet_len': packet_length, 'packet_num': 1, 'frame_len': 4})

if __name__ == '__main__':
    gr_unittest.run(qa_packet_headerparser_b, "qa_packet_headerparser_b.xml")


