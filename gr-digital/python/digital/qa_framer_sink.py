#!/usr/bin/env python
#
# Copyright 2012,2013 Free Software Foundation, Inc.
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

default_access_code = '\xAC\xDD\xA4\xE2\xF2\x8C\x20\xFC'

def string_to_1_0_list(s):
    r = []
    for ch in s:
        x = ord(ch)
        for i in range(8):
            t = (x >> i) & 0x1
            r.append(t)
    return r

def to_1_0_string(L):
    return ''.join(map(lambda x: chr(x + ord('0')), L))

class test_framker_sink(gr_unittest.TestCase):

    def setUp(self):
        self.tb = gr.top_block()

    def tearDown(self):
        self.tb = None

    def test_001(self):

        code = (1, 1, 0, 1)
        access_code = to_1_0_string(code)
        header = tuple(2*[0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1]) # len=1
        pad = (0,) * 100
        src_data = code + header + (0,1,0,0,0,0,0,1) + pad
        expected_data = 'A'

        rcvd_pktq = gr.msg_queue()

        src = blocks.vector_source_b(src_data)
        correlator = digital.correlate_access_code_bb(access_code, 0)
        framer_sink = digital.framer_sink_1(rcvd_pktq)
        vsnk = blocks.vector_sink_b()

        self.tb.connect(src, correlator, framer_sink)
        self.tb.connect(correlator, vsnk)
        self.tb.run()

        result_data = rcvd_pktq.delete_head()
        result_data = result_data.to_string()
        self.assertEqual(expected_data, result_data)

    def test_002(self):

        code = tuple(string_to_1_0_list(default_access_code))
        access_code = to_1_0_string(code)
        header = tuple(2*[0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0]) # len=2
        pad = (0,) * 100
        src_data = code + header + (0,1,0,0,1,0,0,0) + (0,1,0,0,1,0,0,1) + pad
        expected_data = 'HI'

        rcvd_pktq = gr.msg_queue()

        src = blocks.vector_source_b(src_data)
        correlator = digital.correlate_access_code_bb(access_code, 0)
        framer_sink = digital.framer_sink_1(rcvd_pktq)
        vsnk = blocks.vector_sink_b()

        self.tb.connect(src, correlator, framer_sink)
        self.tb.connect(correlator, vsnk)
        self.tb.run()

        result_data = rcvd_pktq.delete_head()
        result_data = result_data.to_string()
        self.assertEqual(expected_data, result_data)

if __name__ == '__main__':
    gr_unittest.run(test_framker_sink, "test_framker_sink.xml")
        
