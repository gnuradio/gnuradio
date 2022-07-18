from gnuradio import blocks
from gnuradio.math.numpy import add_ff, multiply_const_ff
from gnuradio import gr
from gnuradio import gr_unittest

class test_add_numpy(gr_unittest.TestCase):

    def setUp(self):
        self.tb = gr.flowgraph()
        self.rt = gr.runtime()

    def tearDown(self):
        self.tb = None

    def test_add_cc(self):
        src0 = blocks.vector_source_f([1, 3, 5, 7, 9], False)
        src1 = blocks.vector_source_f([0, 2, 4, 6, 8], False)
        expected_result = [3,15,27,13*3,17*3]
        adder = add_ff()
        mult = multiply_const_ff(3)
        sink = blocks.vector_sink_f()
        self.tb.connect((src0, 0), (adder, 0))
        self.tb.connect((src1, 0), (adder, 1))
        self.tb.connect([adder, mult, sink])
        self.rt.initialize(self.tb)
        self.rt.run()
        self.assertEqual(expected_result, sink.data())


if __name__ == '__main__':
    gr_unittest.run(test_add_numpy)