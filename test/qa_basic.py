#!/usr/bin/env python3

from gnuradio import gr_unittest, gr, blocks, streamops

class test_basic(gr_unittest.TestCase):

    def setUp(self):
        self.tb = gr.flowgraph()
        self.rt = gr.runtime()

    def tearDown(self):
        self.tb = None
        self.rt = None

    def test_basic(self):
        nsamples = 100000
        input_data = list(range(nsamples))

        src = blocks.vector_source_f(input_data, False)
        cp1 = streamops.copy(gr.sizeof_float)
        cp2 = streamops.copy(gr.sizeof_float)
        snk1 = blocks.vector_sink_f()
        snk2 = blocks.vector_sink_f()

        self.tb.connect(src, 0, cp1, 0)
        self.tb.connect(cp1, 0, snk1, 0)
        self.tb.connect(src, 0, cp2, 0)
        self.tb.connect(cp2, 0, snk2, 0)

        self.rt.initialize(self.tb)
        self.rt.run()
        
        self.assertEqual(input_data, snk1.data())
        self.assertEqual(input_data, snk2.data())

if __name__ == "__main__":
    gr_unittest.run(test_basic)