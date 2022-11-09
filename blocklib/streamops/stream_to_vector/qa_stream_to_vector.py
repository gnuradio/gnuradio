#!/usr/bin/env python3
#
# Copyright 2022 Block Author
#
# SPDX-License-Identifier: GPL-3.0-or-later
#

from gnuradio import gr, gr_unittest, blocks, streamops

class test_stream_to_vector(gr_unittest.TestCase):
    def setUp(self):
        self.tb = gr.flowgraph()

    def tearDown(self):
        self.tb = None

    def test_instantiate(self):
        op = streamops.stream_to_vector(10)

    def test_001_float(self):
        vlen = 10
        N = 333*vlen
        data = list(range(N))
        src = blocks.vector_source_f(data)
        op = streamops.stream_to_vector(vlen)
        snk = blocks.vector_sink_f(vlen=vlen)
        tb = gr.flowgraph()
        tb.connect([src,op,snk])
        tb.run()
        dst_data = snk.data()

        self.assertFloatTuplesAlmostEqual(data, dst_data)

    def test_002_complex(self):
        vlen = 10
        N = 333*vlen
        data = [x+-x*1j for x in range(N)]
        src = blocks.vector_source_c(data)
        op = streamops.stream_to_vector(vlen)
        snk = blocks.vector_sink_c(vlen=vlen)
        tb = gr.flowgraph()
        tb.connect([src,op,snk])
        tb.run()
        dst_data = snk.data()

        self.assertFloatTuplesAlmostEqual(data, dst_data)

if __name__ == '__main__':
    gr_unittest.run(test_stream_to_vector)

