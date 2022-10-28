#!/usr/bin/env python3
#
# Copyright 2022 Block Author
#
# SPDX-License-Identifier: GPL-3.0-or-later
#

from gnuradio import gr, gr_unittest, blocks, streamops


class test_streams_to_vector(gr_unittest.TestCase):
    def setUp(self):
        self.tb = gr.flowgraph()

    def tearDown(self):
        self.tb = None

    def test_instantiate(self):
        # If this is a templated block, be sure to add the appropriate suffix
        op = streamops.streams_to_vector(10)

    def test_001_float(self):
        vlen = 10
        N = 333*vlen
        data = list(range(N))

        op = streamops.streams_to_vector(vlen)
        tb = gr.flowgraph()
        for ii in range(vlen):
            indata = data[ii::vlen]
            src = blocks.vector_source_f(indata)
            tb.connect(src, 0, op, ii)

        snk = blocks.vector_sink_f(vlen=vlen)

        tb.connect(op, snk)
        tb.run()
        dst_data = snk.data()

        self.assertFloatTuplesAlmostEqual(data, dst_data)

    def test_001_complex(self):
        vlen = 10
        N = 333*vlen
        data = list(range(N))

        op = streamops.streams_to_vector(vlen)
        tb = gr.flowgraph()
        for ii in range(vlen):
            indata = data[ii::vlen]
            src = blocks.vector_source_c(indata)
            tb.connect(src, 0, op, ii)

        snk = blocks.vector_sink_c(vlen=vlen)

        tb.connect(op, snk)
        tb.run()
        dst_data = snk.data()

        self.assertFloatTuplesAlmostEqual(data, dst_data)


if __name__ == '__main__':
    gr_unittest.run(test_streams_to_vector)
