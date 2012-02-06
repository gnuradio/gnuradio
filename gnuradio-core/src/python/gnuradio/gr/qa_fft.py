#!/usr/bin/env python
#
# Copyright 2008,2010 Free Software Foundation, Inc.
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
# You should have received a copy of the GNU General Public License along
# with this program; if not, write to the Free Software Foundation, Inc.,
# 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
#

from gnuradio import gr, gr_unittest
import sys
import random

primes = (2,3,5,7,11,13,17,19,23,29,31,37,41,43,47,53,
          59,61,67,71,73,79,83,89,97,101,103,107,109,113,127,131,
          137,139,149,151,157,163,167,173,179,181,191,193,197,199,211,223,
          227,229,233,239,241,251,257,263,269,271,277,281,283,293,307,311)


class test_fft(gr_unittest.TestCase):

    def setUp(self):
	pass

    def tearDown(self):
	pass

    def assert_fft_ok2(self, expected_result, result_data):
        expected_result = expected_result[:len(result_data)]
        self.assertComplexTuplesAlmostEqual2 (expected_result, result_data,
                                              abs_eps=1e-9, rel_eps=4e-4)

    def assert_fft_float_ok2(self, expected_result, result_data, abs_eps=1e-9, rel_eps=4e-4):
        expected_result = expected_result[:len(result_data)]
        self.assertFloatTuplesAlmostEqual2 (expected_result, result_data,
                                            abs_eps, rel_eps)

    def test_001(self):
	tb = gr.top_block()
        fft_size = 32
        src_data = tuple([complex(primes[2*i], primes[2*i+1]) for i in range(fft_size)])

        expected_result = ((4377+4516j),
                           (-1706.1268310546875+1638.4256591796875j),
                           (-915.2083740234375+660.69427490234375j),
                           (-660.370361328125+381.59600830078125j),
                           (-499.96044921875+238.41630554199219j),
                           (-462.26748657226562+152.88948059082031j),
                           (-377.98440551757812+77.5928955078125j),
                           (-346.85821533203125+47.152004241943359j),
                           (-295+20j),
                           (-286.33609008789062-22.257017135620117j),
                           (-271.52999877929688-33.081821441650391j),
                           (-224.6358642578125-67.019538879394531j),
                           (-244.24473571777344-91.524826049804688j),
                           (-203.09068298339844-108.54627227783203j),
                           (-198.45195007324219-115.90768432617188j),
                           (-182.97744750976562-128.12318420410156j),
                           (-167-180j),
                           (-130.33688354492188-173.83778381347656j),
                           (-141.19784545898438-190.28807067871094j),
                           (-111.09677124023438-214.48896789550781j),
                           (-70.039543151855469-242.41630554199219j),
                           (-68.960540771484375-228.30015563964844j),
                           (-53.049201965332031-291.47097778320312j),
                           (-28.695289611816406-317.64553833007812j),
                           (57-300j),
                           (45.301143646240234-335.69509887695312j),
                           (91.936195373535156-373.32437133789062j),
                           (172.09465026855469-439.275146484375j),
                           (242.24473571777344-504.47515869140625j),
                           (387.81732177734375-666.6788330078125j),
                           (689.48553466796875-918.2142333984375j),
                           (1646.539306640625-1694.1956787109375j))

        src = gr.vector_source_c(src_data)
        s2v = gr.stream_to_vector(gr.sizeof_gr_complex, fft_size)
        fft = gr.fft_vcc(fft_size, True, [], False)
        v2s = gr.vector_to_stream(gr.sizeof_gr_complex, fft_size)
        dst = gr.vector_sink_c()
        tb.connect(src, s2v, fft, v2s, dst)
        tb.run()
        result_data = dst.data()
        #print 'expected:', expected_result
        #print 'results: ', result_data
        #self.assertComplexTuplesAlmostEqual (expected_result, result_data, 5)
        self.assert_fft_ok2(expected_result, result_data)

    def test_002(self):
	tb = gr.top_block()
        fft_size = 32

        tmp_data = ((4377+4516j),
                    (-1706.1268310546875+1638.4256591796875j),
                    (-915.2083740234375+660.69427490234375j),
                    (-660.370361328125+381.59600830078125j),
                    (-499.96044921875+238.41630554199219j),
                    (-462.26748657226562+152.88948059082031j),
                    (-377.98440551757812+77.5928955078125j),
                    (-346.85821533203125+47.152004241943359j),
                    (-295+20j),
                    (-286.33609008789062-22.257017135620117j),
                    (-271.52999877929688-33.081821441650391j),
                    (-224.6358642578125-67.019538879394531j),
                    (-244.24473571777344-91.524826049804688j),
                    (-203.09068298339844-108.54627227783203j),
                    (-198.45195007324219-115.90768432617188j),
                    (-182.97744750976562-128.12318420410156j),
                    (-167-180j),
                    (-130.33688354492188-173.83778381347656j),
                    (-141.19784545898438-190.28807067871094j),
                    (-111.09677124023438-214.48896789550781j),
                    (-70.039543151855469-242.41630554199219j),
                    (-68.960540771484375-228.30015563964844j),
                    (-53.049201965332031-291.47097778320312j),
                    (-28.695289611816406-317.64553833007812j),
                    (57-300j),
                    (45.301143646240234-335.69509887695312j),
                    (91.936195373535156-373.32437133789062j),
                    (172.09465026855469-439.275146484375j),
                    (242.24473571777344-504.47515869140625j),
                    (387.81732177734375-666.6788330078125j),
                    (689.48553466796875-918.2142333984375j),
                    (1646.539306640625-1694.1956787109375j))

        src_data = tuple([x/fft_size for x in tmp_data])

        expected_result = tuple([complex(primes[2*i], primes[2*i+1]) for i in range(fft_size)])

        src = gr.vector_source_c(src_data)
        s2v = gr.stream_to_vector(gr.sizeof_gr_complex, fft_size)
        fft = gr.fft_vcc(fft_size, False, [], False)
        v2s = gr.vector_to_stream(gr.sizeof_gr_complex, fft_size)
        dst = gr.vector_sink_c()
        tb.connect(src, s2v, fft, v2s, dst)
        tb.run()
        result_data = dst.data()
        #print 'expected:', expected_result
        #print 'results: ', result_data
        #self.assertComplexTuplesAlmostEqual (expected_result, result_data, 5)
        self.assert_fft_ok2(expected_result, result_data)

    def test_003(self):
        # Same test as above, only use 2 threads

	tb = gr.top_block()
        fft_size = 32

        tmp_data = ((4377+4516j),
                    (-1706.1268310546875+1638.4256591796875j),
                    (-915.2083740234375+660.69427490234375j),
                    (-660.370361328125+381.59600830078125j),
                    (-499.96044921875+238.41630554199219j),
                    (-462.26748657226562+152.88948059082031j),
                    (-377.98440551757812+77.5928955078125j),
                    (-346.85821533203125+47.152004241943359j),
                    (-295+20j),
                    (-286.33609008789062-22.257017135620117j),
                    (-271.52999877929688-33.081821441650391j),
                    (-224.6358642578125-67.019538879394531j),
                    (-244.24473571777344-91.524826049804688j),
                    (-203.09068298339844-108.54627227783203j),
                    (-198.45195007324219-115.90768432617188j),
                    (-182.97744750976562-128.12318420410156j),
                    (-167-180j),
                    (-130.33688354492188-173.83778381347656j),
                    (-141.19784545898438-190.28807067871094j),
                    (-111.09677124023438-214.48896789550781j),
                    (-70.039543151855469-242.41630554199219j),
                    (-68.960540771484375-228.30015563964844j),
                    (-53.049201965332031-291.47097778320312j),
                    (-28.695289611816406-317.64553833007812j),
                    (57-300j),
                    (45.301143646240234-335.69509887695312j),
                    (91.936195373535156-373.32437133789062j),
                    (172.09465026855469-439.275146484375j),
                    (242.24473571777344-504.47515869140625j),
                    (387.81732177734375-666.6788330078125j),
                    (689.48553466796875-918.2142333984375j),
                    (1646.539306640625-1694.1956787109375j))

        src_data = tuple([x/fft_size for x in tmp_data])

        expected_result = tuple([complex(primes[2*i], primes[2*i+1]) for i in range(fft_size)])

        nthreads = 2

        src = gr.vector_source_c(src_data)
        s2v = gr.stream_to_vector(gr.sizeof_gr_complex, fft_size)
        fft = gr.fft_vcc(fft_size, False, [], False, nthreads)
        v2s = gr.vector_to_stream(gr.sizeof_gr_complex, fft_size)
        dst = gr.vector_sink_c()
        tb.connect(src, s2v, fft, v2s, dst)
        tb.run()
        result_data = dst.data()
        self.assert_fft_ok2(expected_result, result_data)

if __name__ == '__main__':
    gr_unittest.run(test_fft, "test_fft.xml")
        
