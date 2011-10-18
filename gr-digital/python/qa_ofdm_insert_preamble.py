#!/usr/bin/env python
#
# Copyright 2007,2010,2011 Free Software Foundation, Inc.
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
from pprint import pprint
import digital_swig

class test_ofdm_insert_preamble (gr_unittest.TestCase):

    def setUp (self):
        self.tb = gr.top_block ()

    def tearDown (self):
        self.tb = None

    def helper(self, v0, v1, fft_length, preamble):
        tb = self.tb
        src0 = gr.vector_source_c(v0)
        src1 = gr.vector_source_b(v1)
        
        s2v = gr.stream_to_vector(gr.sizeof_gr_complex, fft_length)

        # print "len(v) = %d" % (len(v))

        op = digital_swig.ofdm_insert_preamble(fft_length, preamble)

        v2s = gr.vector_to_stream(gr.sizeof_gr_complex, fft_length)
        dst0 = gr.vector_sink_c()
        dst1 = gr.vector_sink_b()

        tb.connect(src0, s2v, (op, 0))
        tb.connect(src1, (op, 1))
        tb.connect((op, 0), v2s, dst0)
        tb.connect((op, 1), dst1)

        tb.run()
        r0 = dst0.data()
        r0v = []
        for i in range(len(r0)//fft_length):
            r0v.append(r0[i*fft_length:(i+1)*fft_length])
            
        r1 = dst1.data()
        self.assertEqual(len(r0v), len(r1))
        return (r1, r0v)
        
    def check_match(self, actual, expected_list):
        lst = []
        map(lambda x: lst.append(x), expected_list)
        self.assertEqual(actual, lst)


    # ----------------------------------------------------------------

    def test_000(self):
        # no preamble, 1 symbol payloads

        preamble = ()
        fft_length = 8
        npayloads = 8
        v = []
        p = []
        for i in range(npayloads):
            t = fft_length*[(i + i*1j)]
            p.append(tuple(t))
            v += t

        p = tuple(p)
            
        r = self.helper(v, npayloads*[1], fft_length, preamble)
        # pprint(r)

        self.assertEqual(r[0], tuple(npayloads*[1]))
        self.check_match(r[1], (p[0],p[1],p[2],p[3],p[4],p[5],p[6],p[7]))
            

    def test_001(self):
        # 1 symbol preamble, 1 symbol payloads
        preamble = ((100, 101, 102, 103, 104, 105, 106, 107),)
        p0 = preamble[0]
        fft_length = 8
        npayloads = 8
        v = []
        p = []
        for i in range(npayloads):
            t = fft_length*[(i + i*1j)]
            p.append(tuple(t))
            v += t

        
        r = self.helper(v, npayloads*[1], fft_length, preamble)

        self.assertEqual(r[0], tuple(npayloads*[1, 0]))
        self.check_match(r[1], (p0, p[0],
                                p0, p[1],
                                p0, p[2],
                                p0, p[3],
                                p0, p[4],
                                p0, p[5],
                                p0, p[6],
                                p0, p[7]))

    def test_002(self):
        # 2 symbol preamble, 1 symbol payloads
        preamble = ((100, 101, 102, 103, 104, 105, 106, 107),
                    (200, 201, 202, 203, 204, 205, 206, 207))
        p0 = preamble[0]
        p1 = preamble[1]

        fft_length = 8
        npayloads = 8
        v = []
        p = []
        for i in range(npayloads):
            t = fft_length*[(i + i*1j)]
            p.append(tuple(t))
            v += t
        
        r = self.helper(v, npayloads*[1], fft_length, preamble)

        self.assertEqual(r[0], tuple(npayloads*[1, 0, 0]))
        self.check_match(r[1], (p0, p1, p[0],
                                p0, p1, p[1],
                                p0, p1, p[2],
                                p0, p1, p[3],
                                p0, p1, p[4],
                                p0, p1, p[5],
                                p0, p1, p[6],
                                p0, p1, p[7]))


    def xtest_003_preamble(self):
        # 2 symbol preamble, 2 symbol payloads
        preamble = ((100, 101, 102, 103, 104, 105, 106, 107),
                    (200, 201, 202, 203, 204, 205, 206, 207))
        p0 = preamble[0]
        p1 = preamble[1]

        fft_length = 8
        npayloads = 8
        v = []
        p = []
        for i in range(npayloads * 2):
            t = fft_length*[(i + i*1j)]
            p.append(tuple(t))
            v += t
        
        r = self.helper(v, npayloads*[1, 0], fft_length, preamble)

        self.assertEqual(r[0], tuple(npayloads*[1, 0, 0, 0]))
        self.check_match(r[1], (p0, p1, p[0],  p[1],
                                p0, p1, p[2],  p[3],
                                p0, p1, p[4],  p[5],
                                p0, p1, p[6],  p[7],
                                p0, p1, p[8],  p[9],
                                p0, p1, p[10], p[11],
                                p0, p1, p[12], p[13],
                                p0, p1, p[14], p[15]))


if __name__ == '__main__':
    gr_unittest.run(test_ofdm_insert_preamble, "test_ofdm_insert_preamble.xml")
