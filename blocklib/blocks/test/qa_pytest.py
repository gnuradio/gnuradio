#!/usr/bin/env python3

from gnuradio import gr, gr_unittest, blocks


class test_pytest(gr_unittest.TestCase):

    def setUp(self):
        self.tb = gr.flowgraph()

    def tearDown(self):
        self.tb = None

    def test_000(self):
        tb = self.tb
        src_data = [float(x) for x in range(0, 100)]
        expected_result = src_data

        src = blocks.vector_source_f(src_data)
        op = blocks.pytest_numpy()
        dst = blocks.vector_sink_f()

        tb.connect(src, op)
        tb.connect(op, dst)
        tb.run()
        dst_data = dst.data()
        self.assertEqual(expected_result, dst_data)

if __name__ == '__main__':
    gr_unittest.run(test_pytest)
