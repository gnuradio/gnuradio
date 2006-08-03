#!/usr/bin/env python

from gnuradio import gr, gr_unittest
import howto

class qa_howto (gr_unittest.TestCase):

    def setUp (self):
        self.fg = gr.flow_graph ()

    def tearDown (self):
        self.fg = None

    def test_001_square_ff (self):
        src_data = (-3, 4, -5.5, 2, 3)
        expected_result = (9, 16, 30.25, 4, 9)
        src = gr.vector_source_f (src_data)
        sqr = howto.square_ff ()
        dst = gr.vector_sink_f ()
        self.fg.connect (src, sqr)
        self.fg.connect (sqr, dst)
        self.fg.run ()
        result_data = dst.data ()
        self.assertFloatTuplesAlmostEqual (expected_result, result_data, 6)
        
if __name__ == '__main__':
    gr_unittest.main ()
