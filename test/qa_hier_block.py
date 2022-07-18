#!/usr/bin/env python3

from gnuradio import gr_unittest, gr, blocks, streamops

class hwrap(gr.hier_block):
    def __init__(self):
        gr.hier_block.__init__(self, 'hwrap')

        self.add_port_f("hin", gr.INPUT)
        self.add_port_f("hout1", gr.OUTPUT)
        self.add_port_f("hout2", gr.OUTPUT)

        cp1 = streamops.copy(gr.sizeof_float)
        cp2 = streamops.copy(gr.sizeof_float)

        self.connect(self, 0, cp1, 0)
        self.connect(self, 0, cp2, 0)
        self.connect(cp1, 0, self, 0)
        self.connect(cp2, 0, self, 1)

class test_hier(gr_unittest.TestCase):

    def setUp(self):
        self.tb = gr.flowgraph()

    def tearDown(self):
        self.tb = None

    def test_001(self):
        nsamples = 100000
        input_data = list(range(nsamples))

        src = blocks.vector_source_f(input_data, False)
        hblock = hwrap()
        snk1 = blocks.vector_sink_f()
        snk2 = blocks.vector_sink_f()

        self.tb.connect(src, 0, hblock, 0)
        self.tb.connect(hblock, 0, snk1, 0)
        self.tb.connect(src, 0, hblock, 0)
        self.tb.connect(hblock, 1, snk2, 0)

        self.tb.run()
        
        self.assertEqual(input_data, snk1.data())
        self.assertEqual(input_data, snk2.data())


    # TODO: Combinations of Message and Stream ports at input and output

if __name__ == "__main__":
    gr_unittest.run(test_hier)